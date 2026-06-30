// ============================================================================
//  OCP_followed.cpp  —  Open/Closed Principle (OCP) ka SAHI tareeka
// ----------------------------------------------------------------------------
//  Fix: storage ko ek abstract `Persistence` interface ke peeche le aaye.
//  Har naya backend = NAYI class (SQL/Mongo/File...) jo Persistence ko
//  implement kare. Naya backend add karne ke liye purani class ko EDIT nahi
//  karna padta -> "extension ke liye open, modification ke liye closed".
//  (Yeh asal me Strategy pattern hai jo OCP ko enforce karta hai.)
// ============================================================================

#include <iostream>
#include <vector>

using namespace std;

// Product = e-commerce item ka model.
class Product
{
public:
    string name;
    double price;

    Product(string name, double price)
    {
        this->name = name;
        this->price = price;
    }
};

// 1. ShoppingCart: sirf cart business logic.
class ShoppingCart
{
private:
    vector<Product *> products; // heap pe allocate hue products

public:
    // setter
    void addProduct(Product *p)
    {
        products.push_back(p);
    }

    // getter
    const vector<Product *> &getProducts()
    {
        return products;
    }

    double calculateTotal()
    {
        double total = 0;
        for (auto p : products)
        {
            total += p->price;
        }
        return total;
    }
};

// 2. ShoppingCartPrinter: sirf invoice printing.
class ShoppingCartPrinter
{
private:
    ShoppingCart *cart;

public:
    ShoppingCartPrinter(ShoppingCart *cart)
    {
        this->cart = cart;
    }

    void printInvoice()
    {
        cout << "Shopping Cart Invoice:\n";
        for (auto p : cart->getProducts())
        {
            cout << p->name << " - Rs " << p->price << endl;
        }
        cout << "Total: Rs " << cart->calculateTotal() << endl;
    }
};

// Abstract class (sirf declare kiya, define nahi karte).
// Concrete kaam inheritance + polymorphism karega — yahi OCP ka extension point hai.
class Persistence
{
private:
    ShoppingCart *cart;

public:
    virtual void save(ShoppingCart *cart) = 0; // pure virtual -> har backend apna save dega
};

// Naya backend #1 — Persistence ko implement karta hai (purani class edit nahi hui).
class SQLPersistence : public Persistence
{
public:
    void save(ShoppingCart *cart) override
    {
        cout << "Saving shopping cart to SQL DB..." << endl;
    }
};

// Naya backend #2
class MongoPersistence : public Persistence
{
public:
    void save(ShoppingCart *cart) override
    {
        cout << "Saving shopping cart to MongoDB..." << endl;
    }
};

// Naya backend #3 — bas ek aur class, kahin aur kuch change nahi.
class FilePersistence : public Persistence
{
public:
    void save(ShoppingCart *cart) override
    {
        cout << "Saving shopping cart to a file..." << endl;
    }
};

int main()
{
    ShoppingCart *cart = new ShoppingCart();
    cart->addProduct(new Product("Laptop", 50000));
    cart->addProduct(new Product("Mouse", 2000));

    ShoppingCartPrinter *printer = new ShoppingCartPrinter(cart);
    printer->printInvoice();

    // Base pointer `Persistence*` se kisi bhi backend ko polymorphically use karte hain.
    Persistence *db = new SQLPersistence();
    Persistence *mongo = new MongoPersistence();
    Persistence *file = new FilePersistence();

    db->save(cart);    // SQL me save
    mongo->save(cart); // MongoDB me save
    file->save(cart);  // File me save

    return 0;
}
