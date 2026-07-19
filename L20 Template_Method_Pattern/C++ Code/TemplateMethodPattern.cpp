// ============================================================================
//  TemplateMethodPattern.cpp  —  TEMPLATE METHOD PATTERN (Behavioral)
// ----------------------------------------------------------------------------
//  Template Method = "algorithm ka SKELETON (steps ka fixed ORDER) base class
//                     me likho, par kuch steps subclasses ko override karne do.
//                     Order base ke control me, details child ke haath me."
//
//  Asli duniya ka example: chai/coffee banana ☕ (ya yahan: ML pipeline)
//    - Steps ka ORDER fixed: paani garam karo -> ingredient daalo -> chhano
//    - Par "kaunsa ingredient" (chai patti vs coffee) alag hota hai
//    Yahan: ML model train karna — order fixed (load -> preprocess -> train
//    -> evaluate -> save), par "kaise train/evaluate" model pe depend karta.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  TEMPLATE METHOD KYUN? — bina iske code duplication:                    │
//  │                                                                          │
//  │    class NeuralNetTrainer {                                             │
//  │        void train() { load(); preprocess(); trainNN(); eval(); save();} │
//  │    };                                                                    │
//  │    class DecisionTreeTrainer {                                          │
//  │        void train() { load(); preprocess(); trainDT(); eval(); save();} │
//  │        //             ^^^^^^ SAME 4 steps copy-paste! sirf train alag   │
//  │    };                                                                    │
//  │                                                                          │
//  │  load/preprocess/save har trainer me DOBARA likhe — DRY violation.      │
//  │  Aur order galti se badal gaya kisi ek me? Inconsistent behavior!       │
//  │                                                                          │
//  │  TEMPLATE METHOD se: SKELETON (trainPipeline) base me EK BAAR likha —   │
//  │  order guaranteed same. Sirf jo VARY karta hai (trainModel/evaluate)    │
//  │  wo subclass override karti hai. Common steps reuse, unique steps      │
//  │  customize. ✅                                                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  TEEN TARAH KE STEPS hote hain (ye samajh liya to pattern aa gaya):
//    1. CONCRETE (final)    -> loadData() : sab ke liye SAME, override nahi
//    2. HOOK (default)      -> preprocessData(), saveModel() : default hai,
//                              PAR subclass chahe to override kar sakti hai
//    3. ABSTRACT (mandatory)-> trainModel(), evaluateModel() : subclass ko
//                              dena HI padega (pure virtual)
//
//  ⭐ TEMPLATE METHOD vs STRATEGY (interview ka confusion #1):
//    Template Method = INHERITANCE se (subclass steps override kare) —
//                      compile-time pe fix, skeleton base me
//    Strategy        = COMPOSITION se (behavior object inject) — runtime swap
//    Dono "kuch fix, kuch vary" karte hain, par mechanism alag!
// ============================================================================
#include <iostream>
#include <string>

using namespace std;

// ───────────────────────────────────────────────────────────
// 1. Base class defining the template method
// ───────────────────────────────────────────────────────────
class ModelTrainer {
public:
    // >>> YAHI "TEMPLATE METHOD" HAI <<< (pattern ka naam isi se aaya)
    // Ye method ALGORITHM KA SKELETON hai — 5 steps ka FIXED order.
    // Subclass ise override NAHI karti (order badalne ka rasta band).
    // Ye 5 steps me se kuch fixed hain, kuch subclass fill karti hai —
    // par SEQUENCE hamesha yahi rahega. Isi liye ise base me rakha.
    void trainPipeline(const string& dataPath) {
        loadData(dataPath);   // STEP 1: concrete (sab ke liye same)
        preprocessData();     // STEP 2: hook (default, override optional)
        trainModel();         // STEP 3: abstract (subclass ko dena HI hoga)
        evaluateModel();      // STEP 4: abstract (subclass ko dena HI hoga)
        saveModel();          // STEP 5: hook (default, override optional)
    }

protected:
    // CONCRETE step — har trainer ke liye BILKUL SAME. Isliye virtual bhi
    // nahi (override ka koi matlab nahi). Common logic ek jagah = DRY.
    void loadData(const string& path) {
        cout << "[Common] Loading dataset from " << path << "\n";
        // e.g., read CSV, images, etc.
    }

    // HOOK step — default behavior deta hai, PAR virtual hai to subclass
    // chahe to override kar sakti hai. "Optional customization point."
    virtual void preprocessData() {
        cout << "[Common] Splitting into train/test and normalizing\n";
    }

    // ABSTRACT steps (pure virtual) — inka default NAHI hai, subclass ko
    // dena HI padega. Yahi wo "varying parts" hain jinke liye pattern hai.
    virtual void trainModel() = 0;
    virtual void evaluateModel() = 0;

    // HOOK step — default save deta hai, subclass override kar sakti hai
    // (jaise NeuralNet .h5 format me save karega).
    virtual void saveModel() {
        cout << "[Common] Saving model to disk as default format\n";
    }
};

// ───────────────────────────────────────────────────────────
// 2. Concrete subclass: Neural Network
// ───────────────────────────────────────────────────────────
// NeuralNet: teeno customizable steps override karta hai (train + evaluate
// mandatory the, save HOOK ko bhi override kiya kyunki NN ka format alag).
class NeuralNetworkTrainer : public ModelTrainer {
protected:
    void trainModel() override {   // abstract fill
        cout << "[NeuralNet] Training Neural Network for 100 epochs\n";
        // pseudo-code: forward/backward passes, gradient descent...
    }
    void evaluateModel() override {   // abstract fill
        cout << "[NeuralNet] Evaluating accuracy and loss on validation set\n";
    }
    void saveModel() override {   // hook OVERRIDE (default nahi chahiye)
        cout << "[NeuralNet] Serializing network weights to .h5 file\n";
    }
};

// ───────────────────────────────────────────────────────────
// 3. Concrete subclass: Decision Tree
// ───────────────────────────────────────────────────────────
// DecisionTree: SIRF mandatory steps override karta hai. preprocessData()
// aur saveModel() ke DEFAULTS use karta hai (hooks ko chhoda) — yahi hook
// ka fayda: jo chahiye wahi customize karo, baaki free me mil jaata hai.
class DecisionTreeTrainer : public ModelTrainer {
protected:
    // preprocessData() override NAHI kiya -> base ka default chalega
    // (train/test split + normalize) — DT ko custom preprocess nahi chahiye.

    void trainModel() override {   // abstract fill
        cout << "[DecisionTree] Building decision tree with max_depth=5\n";
        // pseudo-code: recursive splitting on features...
    }
    void evaluateModel() override {   // abstract fill
        cout << "[DecisionTree] Computing classification report (precision/recall)\n";
    }
    // saveModel() bhi override NAHI kiya -> base ka default format use hoga
};

// ───────────────────────────────────────────────────────────
// 4. Usage
// ───────────────────────────────────────────────────────────
int main() {
    // Dono ke liye SAME method call (trainPipeline) — base pointer se.
    // Skeleton same chalega, par train/evaluate/save alag output denge.
    cout << "=== Neural Network Training ===\n";
    ModelTrainer* nnTrainer = new NeuralNetworkTrainer();
    nnTrainer->trainPipeline("data/images/");

    cout << "\n=== Decision Tree Training ===\n";
    ModelTrainer* dtTrainer = new DecisionTreeTrainer();
    dtTrainer->trainPipeline("data/iris.csv");

    // NOTE: demo me delete nahi kiya (chhota program). Production me delete
    // ya smart pointers, aur base class me virtual destructor chahiye!
    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  === Neural Network Training ===
//  [Common] Loading dataset from data/images/        <- concrete (same)
//  [Common] Splitting into train/test and normalizing <- hook default
//  [NeuralNet] Training Neural Network for 100 epochs  <- abstract (NN)
//  [NeuralNet] Evaluating accuracy and loss...         <- abstract (NN)
//  [NeuralNet] Serializing network weights to .h5 file <- hook OVERRIDDEN
//
//  === Decision Tree Training ===
//  [Common] Loading dataset from data/iris.csv         <- concrete (same)
//  [Common] Splitting into train/test and normalizing  <- hook DEFAULT
//  [DecisionTree] Building decision tree with max_depth=5 <- abstract (DT)
//  [DecisionTree] Computing classification report...      <- abstract (DT)
//  [Common] Saving model to disk as default format     <- hook DEFAULT
//
//  Dekho: [Common] wale steps DONO me same (reuse!); [NeuralNet]/[DecisionTree]
//  wale alag (customize). Aur order HAMESHA wahi — yahi Template Method! ✅
// ============================================================================
