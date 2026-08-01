// ============================================================================
//  SRP_violated.cpp  —  Single Responsibility Principle (SRP) ka VIOLATION
// ----------------------------------------------------------------------------
//  Concept: SRP kehta hai "ek class ke paas badalne ki sirf EK wajah honi
//  chahiye". Yahan ShoppingCart ek saath 3 kaam kar rahi hai (cart logic +
//  invoice printing + database saving) — yani 3 wajah se badal sakti hai.
//  Isliye yeh SRP TODTI hai. Fix version: SRP_followed.cpp me dekho.
// ============================================================================

#include <iostream>
#include <vector>

using namespace std;

// Product = kisi bhi e-commerce item ka simple model (naam + price).
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

// ❌ SRP VIOLATION: ShoppingCart ek se zyada responsibilities sambhal rahi hai.
class ShoppingCart
{
private:
    vector<Product *> products; // cart ke andar ke saare products

public:
    // Responsibility #1 (sahi): cart me product add karna.
    void addProduct(Product *p)
    {
        products.push_back(p);
    }

    const vector<Product *> &getProducts()
    {
        return products;
    }

    // Responsibility #1 (sahi): cart ka total price nikalna — yeh cart ka hi kaam hai.
    double calculateTotal()
    {
        double total = 0;
        for (auto p : products)
        {
            total += p->price;
        }
        return total;
    }

    // ❌ Responsibility #2 (galat): invoice print karna PRESENTATION ka kaam hai.
    //    Ise alag InvoicePrinter class me hona chahiye.
    void printInvoice()
    {
        cout << "Shopping Cart Invoice:\n";
        for (auto p : products)
        {
            cout << p->name << " - Rs " << p->price << endl;
        }
        cout << "Total: Rs " << calculateTotal() << endl;
    }

    // ❌ Responsibility #3 (galat): DB me save karna PERSISTENCE ka kaam hai.
    //    Ise alag Repository/DAO class me hona chahiye.
    void saveToDatabase()
    {
        cout << "Saving shopping cart to database..." << endl;
    }
};

int main()
{
    // Driver: cart banao, products daalo, phir invoice + DB save.
    // Dhyaan do — agar invoice format YA db logic badle, to ShoppingCart
    // class hi edit karni padegi. Yahi SRP break ka asli nuksaan hai.
    ShoppingCart *cart = new ShoppingCart();

    cart->addProduct(new Product("Laptop", 50000));
    cart->addProduct(new Product("SSD", 8000));
    cart->addProduct(new Product("Mouse", 2000));

    cart->printInvoice();
    cart->saveToDatabase();

    return 0;
}
