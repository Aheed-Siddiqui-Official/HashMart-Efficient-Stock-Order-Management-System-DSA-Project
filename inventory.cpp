#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>

using namespace std;

// Structure for an inventory item (doubly linked list node)
struct InventoryItem {
    int id;
    string name;
    int quantity;
    double price;
    InventoryItem* prev;
    InventoryItem* next;
    InventoryItem(int _id, const string& _name, int _qty, double _price)
        : id(_id), name(_name), quantity(_qty), price(_price), prev(nullptr), next(nullptr) {}
};

// Doubly linked list for inventory
class InventoryList {
private:
    InventoryItem* head;
    InventoryItem* tail;
    unordered_map<int, InventoryItem*> itemMap; // fast lookup by ID

public:
    InventoryList() : head(nullptr), tail(nullptr) {}

    // Add new item or update existing
    void addOrUpdateItem(int id, const string& name, int qty, double price) {
        if (itemMap.count(id)) {
            InventoryItem* node = itemMap[id];
            node->quantity += qty;
            node->price = price;
            cout << "Updated item ID " << id << " with new quantity " << node->quantity << " and price " << price << "\n";
        } else {
            InventoryItem* newNode = new InventoryItem(id, name, qty, price);
            if (!head) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
            itemMap[id] = newNode;
            cout << "Added new item: " << name << " (ID: " << id << ")\n";
        }
    }

    // Remove item completely
    void removeItem(int id) {
        if (!itemMap.count(id)) {
            cout << "Item ID " << id << " not found.\n";
            return;
        }
        InventoryItem* node = itemMap[id];
        if (node->prev) node->prev->next = node->next;
        if (node->next) node->next->prev = node->prev;
        if (node == head) head = node->next;
        if (node == tail) tail = node->prev;
        itemMap.erase(id);
        cout << "Removed item ID " << id << " (" << node->name << ")\n";
        delete node;
    }

    // Reduce quantity when sold; remove if quantity becomes 0
    void reduceQuantity(int id, int soldQty) {
        if (!itemMap.count(id)) {
            cout << "Item ID " << id << " not found.\n";
            return;
        }
        InventoryItem* node = itemMap[id];
        if (soldQty > node->quantity) soldQty = node->quantity;
        node->quantity -= soldQty;
        cout << "Sold " << soldQty << " of item ID " << id << ". Remaining: " << node->quantity << "\n";
        if (node->quantity == 0) {
            cout << "Item ID " << id << " is sold out. Removing from inventory.\n";
            removeItem(id);
        }
    }

    // Display all inventory items
    void displayInventory() const {
        if (!head) {
            cout << "Inventory is empty.\n";
            return;
        }
        cout << "Current Inventory:\n";
        cout << "ID\tName\tQuantity\tPrice\n";
        for (InventoryItem* curr = head; curr; curr = curr->next) {
            cout << curr->id << "\t" << curr->name << "\t" << curr->quantity << "\t" << curr->price << "\n";
        }
    }

    // Lookup item by ID
    InventoryItem* getItem(int id) const {
        if (itemMap.count(id)) return itemMap.at(id);
        return nullptr;
    }
};

// Structure for an order
struct Order {
    int orderID;
    int itemID;
    int quantity;
    int priority; // Lower number = higher priority (min-heap behavior)
    Order(int _orderID, int _itemID, int _qty, int _priority)
        : orderID(_orderID), itemID(_itemID), quantity(_qty), priority(_priority) {}
};

// Comparator for min-heap based on priority; if equal priority, lower orderID first
struct CompareOrder {
    bool operator()(const Order& a, const Order& b) {
        if (a.priority == b.priority)
            return a.orderID > b.orderID;
        return a.priority > b.priority;
    }
};

// Order manager using priority queue
class OrderManager {
private:
    priority_queue<Order, vector<Order>, CompareOrder> pq;
    int nextOrderID;

public:
    OrderManager() : nextOrderID(1) {}

    // Place a new order
    void placeOrder(int itemID, int qty, int priority) {
        Order newOrder(nextOrderID++, itemID, qty, priority);
        pq.push(newOrder);
        cout << "Placed order ID " << newOrder.orderID << " for item ID " << itemID << " (qty: " << qty << ", priority: " << priority << ")\n";
    }

    // Process next order (highest priority = lowest priority number)
    bool processNextOrder(InventoryList& inventory) {
        if (pq.empty()) {
            cout << "No pending orders.\n";
            return false;
        }
        Order top = pq.top();
        pq.pop();
        cout << "Processing order ID " << top.orderID << " (item ID: " << top.itemID << ", qty: " << top.quantity << ", priority: " << top.priority << ")\n";
        InventoryItem* item = inventory.getItem(top.itemID);
        if (!item) {
            cout << "Item ID " << top.itemID << " not available in inventory. Order cannot be fulfilled.\n";
            return false;
        }
        if (top.quantity > item->quantity) {
            cout << "Not enough quantity for item ID " << top.itemID << ". Available: " << item->quantity << ".\n";
            cout << "Order partially fulfilled: Selling " << item->quantity << ". Remaining unfulfilled: " << (top.quantity - item->quantity) << "\n";
            inventory.reduceQuantity(top.itemID, item->quantity);
            return true;
        }
        // Enough stock
        inventory.reduceQuantity(top.itemID, top.quantity);
        cout << "Order fulfilled completely.\n";
        return true;
    }

    // Display pending orders
    void displayPendingOrders() const {
        if (pq.empty()) {
            cout << "No pending orders.\n";
            return;
        }
        // Copy to temp to display without modifying
        priority_queue<Order, vector<Order>, CompareOrder> temp = pq;
        cout << "Pending Orders:\n";
        cout << "OrderID\tItemID\tQuantity\tPriority\n";
        while (!temp.empty()) {
            Order o = temp.top(); temp.pop();
            cout << o.orderID << "\t" << o.itemID << "\t" << o.quantity << "\t" << o.priority << "\n";
        }
    }
};

// Text-based menu
void displayMenu() {
    cout << "\n=== Inventory & Order Management ===\n";
    cout << "1. Add or Update Inventory Item\n";
    cout << "2. Remove Inventory Item\n";
    cout << "3. Display Inventory\n";
    cout << "4. Place Order\n";
    cout << "5. Process Next Order\n";
    cout << "6. Display Pending Orders\n";
    cout << "7. Search Item by ID\n";
    cout << "8. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    InventoryList inventory;
    OrderManager orderManager;
    int choice;
    while (true) {
        displayMenu();
        cin >> choice;
        if (choice == 1) {
            int id, qty;
            string name;
            double price;
            cout << "Enter item ID: "; cin >> id;
            cin.ignore();
            cout << "Enter item name: "; getline(cin, name);
            cout << "Enter quantity: "; cin >> qty;
            cout << "Enter price: "; cin >> price;
            inventory.addOrUpdateItem(id, name, qty, price);
        } else if (choice == 2) {
            int id;
            cout << "Enter item ID to remove: "; cin >> id;
            inventory.removeItem(id);
        } else if (choice == 3) {
            inventory.displayInventory();
        } else if (choice == 4) {
            int itemID, qty, priority;
            cout << "Enter item ID to order: "; cin >> itemID;
            cout << "Enter quantity: "; cin >> qty;
            cout << "Enter priority (lower number = higher priority): "; cin >> priority;
            orderManager.placeOrder(itemID, qty, priority);
        } else if (choice == 5) {
            orderManager.processNextOrder(inventory);
        } else if (choice == 6) {
            orderManager.displayPendingOrders();
        } else if (choice == 7) {
            int id;
            cout << "Enter item ID to search: "; cin >> id;
            InventoryItem* item = inventory.getItem(id);
            if (item) {
                cout << "Item found: ID " << item->id << ", Name: " << item->name << ", Quantity: " << item->quantity << ", Price: " << item->price << "\n";
            } else {
                cout << "Item ID " << id << " not found.\n";
            }
        } else if (choice == 8) {
            cout << "Exiting...\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}