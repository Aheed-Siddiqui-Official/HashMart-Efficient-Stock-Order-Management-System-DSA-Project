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

