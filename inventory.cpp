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

    