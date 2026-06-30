// ============================================================================
//  SRP_followed.cpp  —  Single Responsibility Principle (SRP) ka SAHI tareeka
// ----------------------------------------------------------------------------
//  Idea: SRP_violated.cpp wali ek moti ShoppingCart ko 3 focused classes me
//  tod diya — har class ke paas badalne ki sirf EK wajah:
//    1) ShoppingCart        -> sirf cart/business logic
//    2) ShoppingCartPrinter -> sirf invoice printing (presentation)
//    3) ShoppingCartStorage -> sirf DB save (persistence)
//  Ab invoice format badle to sirf Printer change hoga, cart nahi.
// ============================================================================

#include <iostream>
#include <vector>

using namespace std;

// Product = e-commerce item ka simple model.
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

// 1. ShoppingCart: sirf cart-related business logic sambhalti hai.
class ShoppingCart
{
private:
    vector<Product *> products; // heap pe allocate hue products store karte hain

public:
    // setter jaisa kaam — cart me product daalna
    void addProduct(Product *p)
    {
        products.push_back(p);
    }

    // getter jaisa kaam — saare products read karne ke liye
    const vector<Product *> &getProducts()
    {
        return products;
    }

    // cart ka total price — yeh cart ki apni responsibility hai (sahi jagah)
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

// 2. ShoppingCartPrinter: sirf invoice print karne ki responsibility.
//    Cart ka reference rakhta hai, par cart ki logic me dakhal nahi deta.
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

// 3. ShoppingCartStorage: sirf cart ko DB me save karne ki responsibility.
class ShoppingCartStorage
{
private:
    ShoppingCart *cart;

public:
    ShoppingCartStorage(ShoppingCart *cart)
    {
        this->cart = cart;
    }

    void saveToDatabase()
    {
        cout << "Saving shopping cart to database..." << endl;
    }
};

int main()
{
    // Driver: ek cart, par alag-alag objects har responsibility ke liye.
    ShoppingCart *cart = new ShoppingCart();

    cart->addProduct(new Product("Laptop", 50000));
    cart->addProduct(new Product("Mouse", 2000));
    cart->addProduct(new Product("SSD", 8000));

    // Printing alag class se
    ShoppingCartPrinter *printer = new ShoppingCartPrinter(cart);
    printer->printInvoice();

    // DB save alag class se
    ShoppingCartStorage *db = new ShoppingCartStorage(cart);
    db->saveToDatabase();

    return 0;
}
