// ============================================================================
//  IteratorPattern.cpp  —  Iterator Design Pattern (Behavioral)
// ----------------------------------------------------------------------------
//  Iterator = kisi collection ko traverse karne ka UNIFORM tareeka, uski
//  internal structure ko expose kiye bina. Client sirf hasNext()/next() jaanta
//  hai. Yahan teen alag structures — LinkedList, BinaryTree (in-order via
//  stack), Playlist — sab ek hi `Iterator<T>` interface dete hain, isliye
//  ek hi loop sabpe chalta hai. Cursor iterator ke andar rehta hai (multiple
//  independent traversals possible).
//
//  Pattern ke 4 roles (GoF naming) is file me kaun nibha raha hai:
//    1. Iterator (interface)          -> class Iterator<T>
//    2. ConcreteIterator (actual walk)-> LinkedListIterator, BinaryTreeInorderIterator,
//                                        PlaylistIterator
//    3. Iterable / Aggregate (iface)  -> class Iterable<T>
//    4. ConcreteAggregate (collection)-> LinkedList, BinaryTree, Playlist
//
//  Fayda kya hai:
//    - Client ko `->next` pointer, `stack`, ya `vector index` — kuch nahi pata.
//      Traversal ka logic collection se BAHAR nikal ke iterator me chala gaya
//      (Single Responsibility: collection data rakhe, iterator usse ghume).
//    - Ek naya structure add karo (e.g. Graph), naya iterator likh do —
//      client ka `while (it->hasNext())` loop bilkul same rehta hai (Open/Closed).
// ============================================================================
#include <iostream>
#include <vector>
#include <stack> // BinaryTree ka in-order traversal recursion ki jagah explicit stack se
#include <string>

using namespace std;

// ============================================================================
//  Iterator & Iterable Hierrarchy  (pattern ka dil — do chhote interfaces)
// ============================================================================

// Iterator<T> = "cursor" ka contract.
// T templated hai, isliye ek hi interface int, Song, ya kisi bhi type pe chalega.
// Ye class abstract hai (pure virtual functions ke wajah se) — iska object
// direct nahi ban sakta, sirf derive karke hi use hoga.
template <typename T>
class Iterator
{
public:
    // hasNext() -> "aage koi element bacha hai kya?"
    // Ye SIRF poochhta hai, cursor ko aage nahi badhata (no side-effect).
    virtual bool hasNext() = 0;

    // next() -> "current element do AUR cursor ek kadam aage badha do."
    // Yahi do kaam ek saath karta hai: read + advance.
    // Contract: isse call karne se pehle hasNext() true hona chahiye,
    // warna nullptr deref / out-of-range ho jayega.
    virtual T next() = 0;
};

// Iterable<T> = "main traverse ho sakta hoon" ka contract.
// Jo bhi collection is interface ko implement karega, wo apne liye ek
// fresh iterator bana ke de dega. Client ko collection ka andar ka structure
// (linked nodes? tree? vector?) kabhi pata nahi chalta — bas iterator milta hai.
template <typename T>
class Iterable
{
public:
    // Har call pe NAYA iterator return hota hai (fresh cursor at position 0).
    // Isi wajah se ek hi collection pe 2-3 independent traversals ek saath
    // chal sakte hain — kyunki cursor collection me nahi, iterator me hai.
    virtual Iterator<T> *getIterator() = 0;
};

// ============================================================================
//  Concrete Collections (ConcreteAggregate)  — teen bilkul alag data structures
// ============================================================================

// Collection #1: Linked List
// Note: yaha node hi list hai (har node khud ek LinkedList hai, `next` se juda).
// Isliye `list` variable actually HEAD node ko point karta hai.
class LinkedList : public Iterable<int>
{
public:
    int data;         // node ki value
    LinkedList *next; // agle node ka pointer (last node pe nullptr)

    LinkedList(int value)
    {
        data = value;
        next = nullptr; // naya node hamesha "tail" ki tarah start hota hai
    }

    // Declaration abhi, definition neeche —
    // kyunki LinkedListIterator abhi tak declare hi nahi hua (forward dependency).
    Iterator<int> *getIterator() override;
};

// Collection #2: Binary Tree
// Har node ke 2 children. Traversal ka order ambiguous hai (pre / in / post),
// isliye yaha "in-order" wala iterator banaya hai — sorted-ish output ke liye.
class BinaryTree : public Iterable<int>
{
public:
    int data;
    BinaryTree *left;  // chhoti values (BST convention)
    BinaryTree *right; // badi values

    BinaryTree(int value)
    {
        data = value;
        left = nullptr;
        right = nullptr; // leaf node ke roop me start
    }

    Iterator<int> *getIterator() override;
};

// Collection #3 ka element type: Song
// Ye simple data holder (POD-ish) hai — koi iteration logic nahi.
class Song
{
public:
    string title;
    string artist;

    // const& params: unnecessary string copy avoid karne ke liye
    Song(const string &t, const string &a)
    {
        title = t;
        artist = a;
    }
};

// Collection #3: Playlist
// Andar bas ek vector hai. Dhyaan do — client ko `songs` vector directly
// touch karne ki zaroorat nahi, wo iterator se ghumega.
// Isi liye Iterable<Song> — yaani `T` ab `int` nahi, ek custom object hai.
class Playlist : public Iterable<Song>
{
public:
    vector<Song> songs;

    void addSong(const Song &s)
    {
        songs.push_back(s);
    }

    Iterator<Song> *getIterator() override;
};

// ============================================================================
//  Concrete Iterators  — asli traversal logic yahi rehta hai
// ============================================================================

// Iterator #1: Linked List ke liye — sabse seedha wala.
// State = bas ek pointer (`current`). Traversal = pointer ko aage sarkao.
class LinkedListIterator : public Iterator<int>
{
private:
    LinkedList *current; // cursor — abhi kis node pe khade hain

public:
    // Cursor ko HEAD pe rakh diya. Ye iterator ka apna private cursor hai,
    // list khud kuch track nahi kar rahi.
    LinkedListIterator(LinkedList *head)
    {
        current = head;
    }

    // List khatam tab hoti hai jab current nullptr ho jaye (last node ka next).
    bool hasNext() override
    {
        return current->next != nullptr;
    }

    int next() override
    {
        int val = current->data; // 1-> 2-> 3 : pehle CURRENT ki value uthao
        current = current->next; // phir cursor ko agle node pe shift karo
        return val;              // aur purani value return kar do
    }
};

// Iterator #2: Binary Tree ka IN-ORDER traversal (Left -> Root -> Right).
//
// Twist: in-order normally recursion se likhte hain, lekin recursion "ek baar
// me poora tree" ghum jaata hai. Humein to ek-ek element "on demand" chahiye
// (next() call pe ek). Isliye recursion ka call-stack MANUALLY `stack` me
// simulate kar rahe hain — isko "lazy / pausable traversal" kehte hain.
//
// Example tree:      2
//                   / \
//                  1   3      -> output: 1 2 3
class BinaryTreeInorderIterator : public Iterator<int>
{
private:
    stack<BinaryTree *> stk; // pending nodes — jinka data abhi return hona baaki hai
    BinaryTree *curr;        // sirf constructor me starting point ke liye

    // Helper: `node` se shuru karke uske sabse left-most raaste ke
    // saare nodes stack me daal do.
    // Kyun? Kyunki in-order me sabse pehle SABSE LEFT wala node aata hai.
    // Stack LIFO hai, to jo last me push hua (deepest left) wahi pehle nikalega.
    void pushLefts(BinaryTree *node)
    {
        while (node)
        {
            stk.push(node);
            node = node->left;
        }
    }

public:
    BinaryTreeInorderIterator(BinaryTree *root)
    {
        curr = root;
        pushLefts(curr); // stack ko pehle element ke liye "prime" kar diya
                         // 2 ke tree me: stack = [2, 1]  (top = 1)
    }

    // Kaam baaki hai jab tak stack me koi pending node hai.
    bool hasNext() override
    {
        return !stk.empty();
    }

    int next() override
    {
        // Stack ka top = agla in-order element (guaranteed, kyunki uske
        // left side ka sab kaam already ho chuka hai).
        BinaryTree *node = stk.top();
        stk.pop();
        int val = node->data;

        // Ab is node ka left+root ho gaya, to uska RIGHT subtree process karna hai.
        // Right subtree ke liye bhi wahi rule: uske left-most chain ko push karo.
        if (node->right)
        {
            pushLefts(node->right);
        }

        return val;
    }
};

// Iterator #3: Playlist ke liye — index-based cursor.
class PlaylistIterator : public Iterator<Song>
{
private:
    vector<Song> vec; // NOTE: yaha vector ki COPY rakhi hai (snapshot).
                      // Fayda: iterate karte waqt agar koi playlist me song
                      // add/remove kar de, to hamara loop crash nahi hoga
                      // (Java me yahi "ConcurrentModificationException" deta hai).
                      // Nuksaan: bade playlist pe copy costly hai — production me
                      // reference/pointer + version-check better hota hai.
    size_t index;     // cursor = current position (0-based)

public:
    PlaylistIterator(vector<Song> v)
    {
        vec = v;
        index = 0; // cursor pehle gaane pe
    }

    bool hasNext() override
    {
        return index < vec.size(); // index last element ko cross kar gaya? -> khatam
    }

    Song next() override
    {
        // post-increment: pehle vec[index] return hota hai, phir index++ hota hai.
        // Yani "current do, cursor aage badhao" ek hi line me.
        return vec[index++];
    }
};

// ============================================================================
//  Attaching Iterators  — har collection apna matching iterator "factory" karta hai
// ----------------------------------------------------------------------------
//  Definitions yaha (class ke bahar) isliye likhi hain kyunki collections upar
//  declare hue the jab iterator classes exist hi nahi karti thi.
//  Client ko return type sirf `Iterator<T>*` dikhta hai — concrete type chhupa
//  rehta hai. Yahi "programming to an interface" hai.
// ============================================================================

Iterator<int> *LinkedList::getIterator()
{
    // `this` = head node, wahi se traversal start hoga
    return new LinkedListIterator(this);
}

Iterator<int> *BinaryTree::getIterator()
{
    // `this` = root node
    return new BinaryTreeInorderIterator(this);
}

Iterator<Song> *Playlist::getIterator()
{
    // songs vector iterator ko de diya (wo apni copy bana lega)
    return new PlaylistIterator(songs);
}

// ============================================================================
//  Client Code  — asli punchline yaha hai
// ----------------------------------------------------------------------------
//  Neeche teeno loops ko dhyaan se dekho: while (it->hasNext()) it->next();
//  Structure teen alag (list / tree / vector), traversal teen alag (pointer /
//  stack / index) — par CLIENT ka code EK JAISA hai. Yahi Iterator ka pura point.
// ============================================================================
int main()
{

    //------------------------------------------------
    // LinkedList: 1 → 2 → 3
    LinkedList *list = new LinkedList(1);
    list->next = new LinkedList(2);
    list->next->next = new LinkedList(3);

    // Collection se cursor maanga — ab pointer-chasing hamara kaam nahi
    Iterator<int> *iterator1 = list->getIterator();

    cout << "LinkedList contents: ";

    while (iterator1->hasNext())
    {
        cout << iterator1->next() << " ";
    }

    cout << "\n";

    //------------------------------------------------

    // BinaryTree:
    //    2
    //   / \
    //  1   3
    BinaryTree *root = new BinaryTree(2);
    root->left = new BinaryTree(1);
    root->right = new BinaryTree(3);

    Iterator<int> *iterator2 = root->getIterator();

    cout << "BinaryTree inorder: ";

    // Bilkul wahi loop jo LinkedList pe chala tha — par andar stack chal raha hai.
    // Output in-order aayega: 1 2 3
    while (iterator2->hasNext())
    {
        cout << iterator2->next() << " ";
    }

    cout << "\n";

    //------------------------------------------------

    // Playlist
    Playlist playlist;
    playlist.addSong(Song("Admirin You", "Karan Aujla"));
    playlist.addSong(Song("Husn", "Anuv Jain"));

    // Ab T = Song (custom object), par interface wahi ka wahi
    Iterator<Song> *iterator3 = playlist.getIterator();

    cout << "Playlist songs:\n";

    while (iterator3->hasNext())
    {
        Song s = iterator3->next();
        cout << "  " << s.title << " by " << s.artist << "\n";
    }

    //------------------------------------------------

    // Cleanup
    // LinkedList ko ULTA (tail se head) delete kar rahe hain — agar pehle head
    // delete kar dete, to `list->next` padhne ka koi tareeka hi nahi bachta
    // (dangling pointer). Isliye deepest node pehle.
    delete list->next->next;
    delete list->next;
    delete list;

    // Tree me children pehle, root baad me — same logic (bottom-up)
    delete root->left;
    delete root->right;
    delete root;

    // NOTE (interview-worthy): iterator1/2/3 `new` se bane the par delete nahi hue
    // -> ye memory leak hai. Real code me `unique_ptr<Iterator<T>>` return karna
    // chahiye, aur Iterator/Iterable me `virtual ~Iterator() = default;` hona chahiye
    // (warna base pointer se delete karne pe derived ka destructor hi nahi chalega —
    // undefined behavior). Playlist iterator ka `vector<Song>` member to leak hoga hi.

    return 0;
}
