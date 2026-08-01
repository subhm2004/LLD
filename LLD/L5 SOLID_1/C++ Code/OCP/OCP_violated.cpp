// ============================================================================
//  OCP_violated.cpp  —  Open/Closed Principle (OCP) ka VIOLATION
// ----------------------------------------------------------------------------
//  OCP: class "extension ke liye open" par "modification ke liye closed" honi
//  chahiye. Yahan ShoppingCartStorage me har naye storage type ke liye ek NAYA
//  method (saveToSQL/saveToMongo/saveToFile...) add karna padta hai — yani
//  existing tested class baar-baar EDIT hoti hai. Yahi OCP break hai.
//  Fix: OCP_followed.cpp me Persistence abstraction se dekho.
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

// 1. ShoppingCart: sirf cart business logic (SRP yahan sahi hai).
class ShoppingCart
{
private:
    vector<Product *> products;

public:
    void addProduct(Product *p)
    {
        products.push_back(p);
    }

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

// ❌ 3. ShoppingCartStorage: har naye storage backend ke liye yahan NAYA
//    method add karna padega -> class har baar modify hoti hai -> OCP break.
class ShoppingCartStorage
{
private:
    ShoppingCart *cart;

public:
    ShoppingCartStorage(ShoppingCart *cart)
    {
        this->cart = cart;
    }

    void saveToSQLDatabase() // SQL ke liye
    {
        cout << "Saving shopping cart to SQL DB..." << endl;
    }

    void saveToMongoDatabase() // Mongo ke liye -> naya method
    {
        cout << "Saving shopping cart to Mongo DB..." << endl;
    }

    void saveToFile() // File ke liye -> phir naya method (modification)
    {
        cout << "Saving shopping cart to File..." << endl;
    }
};

int main()
{
    ShoppingCart *cart = new ShoppingCart();

    cart->addProduct(new Product("Laptop", 50000));
    cart->addProduct(new Product("Mouse", 2000));

    ShoppingCartPrinter *printer = new ShoppingCartPrinter(cart);
    printer->printInvoice();

    // Har backend ke liye alag method call — naya backend = class edit.
    ShoppingCartStorage *db = new ShoppingCartStorage(cart);
    db->saveToSQLDatabase();
    db->saveToMongoDatabase();
    db->saveToFile();

    return 0;
}
