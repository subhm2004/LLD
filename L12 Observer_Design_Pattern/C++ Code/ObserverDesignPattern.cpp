// yt nodification system ese design kr skte hai
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
// agar kisi abstract class sare methos virtual hai to use hum Interface bolte hai or koi ek bhi method virtual h to use bolte hai abstract class

class ISubscriber
{
public:
    virtual void update() = 0;
    virtual ~ISubscriber() {} // virtual destructor for interface
};

// Abstract Observable interface: a YouTube channel interface
class IChannel
{
public:
    virtual void subscribe(ISubscriber *subscriber) = 0;
    virtual void unsubscribe(ISubscriber *subscriber) = 0;
    virtual void notifySubscribers() = 0;
    virtual ~IChannel() {}
};

// Concrete Subject: a YouTube channel that observers can subscribe to
class Channel : public IChannel
{
private:
    vector<ISubscriber *> subscribers; // list of subscribers
    string name;
    string latestVideo; // latest uploaded video title
public:
    Channel(const string &name)
    {
        this->name = name;
    }

    // Add a subscriber (avoid duplicates)
    void subscribe(ISubscriber *subscriber) override
    {
        if (find(subscribers.begin(), subscribers.end(), subscriber) == subscribers.end())
        {
            subscribers.push_back(subscriber);
        }
    }

    // Remove a subscriber if present
    void unsubscribe(ISubscriber *subscriber) override
    {
        auto it = find(subscribers.begin(), subscribers.end(), subscriber);
        if (it != subscribers.end())
        {
            subscribers.erase(it);
        }
    }

    // Notify all subscribers of the latest video
    void notifySubscribers() override
    {
        for (ISubscriber *sub : subscribers)
        {
            sub->update();
        }
    }

    // Upload a new video and notify all subscribers
    void uploadVideo(const string &title)
    {
        latestVideo = title;
        cout << "\n[" << name << " uploaded \"" << title << "\"]\n";
        notifySubscribers();
    }

    // Read video data
    string getVideoData()
    {
        return "\nCheckout our new Video : " + latestVideo + "\n";
    }
};

// Concrete Observer: represents a subscriber to the channel
class Subscriber : public ISubscriber
{
private:
    string name;
    Channel *channel;

public:
    Subscriber(const string &name, Channel *channel)
    {
        this->name = name;
        this->channel = channel;
    }

    // Called by Channel; prints notification message
    void update() override
    {
        cout << "Hey " << name << "," << this->channel->getVideoData();
    }
};

int main()
{
    // Create a channel and subscribers
    Channel *channel = new Channel("Bhai_ki_padhai");

    Subscriber *subs1 = new Subscriber("Shubham", channel);
    Subscriber *subs2 = new Subscriber("Hardik", channel);

    // Shubham and Hardik subscribe to Bhai_ki_padhai
    channel->subscribe(subs1);
    channel->subscribe(subs2);

    // Upload a video: both Shubham and Hardik are notified
    channel->uploadVideo("Observer Pattern Tutorial");

    // Shubham unsubscribes; Hardik remains subscribed
    channel->unsubscribe(subs1);

    // Upload another video: only Hardik is notified
    channel->uploadVideo("Decorator Pattern Tutorial");

    return 0;
}
