#include <vector>
#include <string>
#include "../include/Workout.h"
#include "../include/Customer.h"
#include <iostream>
#include <algorithm>
#include "../include/Trainer.h"

using namespace std;

Trainer::Trainer(int t_capacity) : capacity(t_capacity) {
    salary = 0;
    WasEverOpen = false;
    open = false;
    customersList = std::vector<Customer *>();
    orderList = std::vector<OrderPair>();
}

//destructor
Trainer::~Trainer() {
    for (auto i: customersList) {
        delete i;
    }
}

//copy constructor
Trainer::Trainer(const Trainer &trainer) : capacity(trainer.capacity), open(trainer.open),
                                           customersList(),
                                           orderList() {
    salary = trainer.salary;
    WasEverOpen = trainer.WasEverOpen;
    for (auto i: trainer.customersList) {
        customersList.push_back(i->clone());
    }
    for (auto i: trainer.orderList) {
        orderList.push_back(i);
    }
}

//copy assigment operator
Trainer &Trainer::operator=(const Trainer &trainer) {
    if (&trainer != this) {
        for (auto i: customersList) {
            delete i;
        }
        std::vector<Customer *> c;
        for (auto i: trainer.customersList) {
            if (i->toString() == "swt") {
                SweatyCustomer *cust = new SweatyCustomer(i->getName(), i->getId());
                c.push_back(cust);
            } else if (i->toString() == "chp") {
                CheapCustomer *cust = new CheapCustomer(i->getName(), i->getId());
                c.push_back(cust);
            } else if (i->toString() == "mcl") {
                HeavyMuscleCustomer *cust = new HeavyMuscleCustomer(i->getName(), i->getId());
                c.push_back(cust);
            } else {
                FullBodyCustomer *cust = new FullBodyCustomer(i->getName(), i->getId());
                c.push_back(cust);
            }
        }
        std::vector<OrderPair> o(trainer.orderList);
        capacity = trainer.capacity;
        WasEverOpen = trainer.WasEverOpen;
        open = trainer.open;
        salary = trainer.salary;
        orderList = std::move(o);
        customersList = c;
    }
    return *this;
}

//move constructor
Trainer::Trainer(Trainer &&t) : capacity(t.capacity), open(t.open), customersList(t.customersList),
                                orderList(t.orderList) {
    salary = t.salary;
    WasEverOpen = t.WasEverOpen;
    // because the destructor is deleted at the end of the operation and we dont want the vector
    //to be deleted we put a new vector (empty one) as t's vector
    t.customersList = std::vector<Customer *>();
}

//move assigment operator
Trainer &Trainer::operator=(Trainer &&t) {
    if (this != &t) {
        for (auto i: customersList) {
            delete i;
        }
        salary = t.salary;
        WasEverOpen = t.WasEverOpen;
        capacity = t.capacity;
        open = t.open;
        orderList = std::move(t.orderList);
        customersList = t.customersList;
        t.orderList = std::vector<OrderPair>();
        t.customersList = std::vector<Customer *>();
    }
    return *this;
}

int Trainer::getCapacity() const {
    return capacity;
}


void Trainer::addCustomer(Customer *customer) {
    size_t cap = capacity;
    bool found = false;
    size_t cus_id = customer->getId();
    std::vector<Customer *> newCL;
    if (customersList.size() < cap) {
        for (Customer *i: customersList) {
            size_t idc = i->getId();
            if (idc < cus_id) {
                newCL.push_back(i);
            }
            if ((cus_id < idc) & !found) {
                newCL.push_back(customer);
                found = true;
            }
            if (found) {
                newCL.push_back(i);
            }
        }
        if (!found) {
            newCL.push_back(customer);
        }
        this->customersList = newCL;
    } else {
        delete customer;
    }
}

void Trainer::removeCustomer(int id) {
    std::vector<Customer *> newV;
    for (Customer *curr: customersList) {
        if (curr->getId() == id) {
            for (Customer *next: customersList) {
                if (next != curr) {
                    newV.push_back(next);
                }
            }
            break;
        }
    }
    customersList = newV;
    size_t i = 0;
    std::vector<OrderPair> newOL;
    while (i < orderList.size()) {
        //no break line because a customer can have a few orders
        if (orderList[i].first == id) {
            salary -= orderList[i].second.getPrice();
        }
        i++;
    }
    for (size_t j = 0; j < orderList.size(); ++j) {
        if (orderList[j].first != id) {
            newOL.push_back(orderList[j]);
        }
    }
    orderList = std::move(newOL);
}

Customer *Trainer::getCustomer(int id) {
    for (size_t i = 0; i < customersList.size(); ++i) {
        if (customersList[i]->getId() == id) {
            return customersList[i];
        }
    }
    return nullptr;
}

std::vector<Customer *> &Trainer::getCustomers() {
    return customersList;
}

std::vector<OrderPair> &Trainer::getOrders() {
    return orderList;
}

void Trainer::order(const int customer_id, const std::vector<int> workout_ids,
                    const std::vector<Workout> &workout_options) {
    for (size_t i = 0; i < workout_ids.size(); ++i) {
        for (size_t j = 0; j < workout_options.size(); ++j) {
            if (workout_ids[i] == workout_options[j].getId()) {
                orderList.push_back(make_pair(customer_id, workout_options[j]));
                salary += (workout_options[j]).getPrice();
            }
        }
    }
}

void Trainer::openTrainer() {
    WasEverOpen = true;
    open = true;
}

void Trainer::closeTrainer() {
    open = false;
    for (Customer *i: customersList) {
        if (i) {
            //added an & to delete the customer itself and not the pointer to the customer
            delete i;
        }
//        for (Customer* &i: customersList) {
//            //added an & to delete the customer itself and not the pointer to the customer
//            delete i;
//        }
    }
    customersList.clear();
    orderList.clear();
}

int Trainer::getSalary() {
    return salary;
}

bool Trainer::isOpen() {
    return open;
}

bool Trainer::getWasEverOpen() {
    return WasEverOpen;
}

Trainer *Trainer::clone() const {
    return new Trainer(*this);
}