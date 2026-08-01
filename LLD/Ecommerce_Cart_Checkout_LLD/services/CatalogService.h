// ============================================================================
//  services/CatalogService.h  —  Products ka store (product master data)
// ----------------------------------------------------------------------------
//  Products add karo, id se nikaalo, category se search karo. Bas itna.
//
//  📌 SRP: ye service sirf product DEFINITION (naam/price/category) jaanti hai —
//     STOCK ka kuch pata nahi (wo InventoryService ka kaam). Do alag concerns:
//     product details kabhi-kabhi badalti hain, stock har order pe badalta hai.
//
//  ⭐ `searchByCategory` `const Product*` (pointers) return karta hai — products
//     ki COPY nahi banti, sirf catalog ke andar wale asli objects ke pate.
//     `const` isliye ki caller unhe modify na kar sake (read-only view).
//
//  ⚠ Linear scan hai (O(N) over all products). Real system me category-wise
//     index / search engine hota. Demo scale pe theek.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CATALOGSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_CATALOGSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/Product.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CatalogService {
public:
    // insert_or_assign -> naya add, ya purana update (dono ek call me).
    void addProduct(const Product &product) {
        products_.insert_or_assign(product.getProductId(), product);
    }

    // Nahi mila -> saaf error (fail-fast, silent nullptr nahi).
    const Product &getProductOrThrow(const string &productId) const {
        auto it = products_.find(productId);
        if (it == products_.end()) {
            throw runtime_error("Product not found: " + productId);
        }
        return it->second;
    }

    // Category ke saare products (pointers — copy nahi).
    vector<const Product *> searchByCategory(const string &category) const {
        vector<const Product *> result;
        for (const auto &entry : products_) {
            if (entry.second.getCategory() == category) {
                result.push_back(&entry.second);
            }
        }
        return result;
    }

private:
    unordered_map<string, Product> products_; // productId -> product (stock yahan NAHI)
};

}  // namespace ecommerce_checkout_lld

#endif
