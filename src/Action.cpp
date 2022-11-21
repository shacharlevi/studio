#include "../include/Action.h"
#include <string>
#include <iostream>
#include "../include/Customer.h"
#include "../include/Trainer.h"
#include "../include/Studio.h"

extern Studio *backup;

using namespace std;

BaseAction::BaseAction() :
        errorMsg(""), status(ERROR) {}

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status = COMPLETED;
}

void BaseAction::error(std::string errorMsg) {
    status = ERROR;
    this->errorMsg = errorMsg;
    std::cout << "Error: " << errorMsg << std::endl;
}

std::string BaseAction::getErrorMsg() const {
    return "Error: " + errorMsg;
}

OpenTrainer::OpenTrainer(int id, std::vector<Customer *> &customersList) :
        trainerId(id), customers(customersList) {}

void OpenTrainer::act(Studio &studio) {
    Trainer *trainer = studio.getTrainer(trainerId);
    if (!trainer || trainer->isOpen()) {
        error("Trainer does not exist or is not open");
        for (Customer *customer: customers) {
            delete customer;
        }
    } else {
        trainer->openTrainer();
        for (Customer *customer: customers) {
            trainer->addCustomer(customer);
        }
        complete();
    }
    studio.addAction(this);
}

// prints "open"+ trainer id  customer 1 name, customer1 strategy...."
std::string OpenTrainer::toString() const {
    std::string ans = "open " + std::to_string(trainerId) + " ";
    if (getStatus() == COMPLETED) {
        for (size_t i = 0; i < customers.size(); ++i) {
            ans += customers[i]->getName() + "," + customers[i]->toString();
        }
        return ans + " Completed";
    } else {
        return ans + " " + getErrorMsg();
    }
}

BaseAction *OpenTrainer::clone() const {
    return new OpenTrainer(*this);
}

Order::Order(int id) : trainerId(id) {
    trainer = nullptr;
}

void Order::act(Studio &studio) {
    trainer = studio.getTrainer(trainerId);
    if (!(trainer) || !(trainer->isOpen())) {
        error("Trainer does not exist or is not open");
    } else {
        std::vector<Workout> wO = studio.getWorkoutOptions();
        for (size_t i = 0; i < trainer->getCustomers().size(); ++i) {
            //Customer id,workout id,workout options
            trainer->order(trainer->getCustomers()[i]->getId(), trainer->getCustomers()[i]->order(wO), wO);
        }
        std::vector<std::pair<int, Workout>> pair = trainer->getOrders();
        for (size_t i = 0; i < pair.size(); ++i) {
            std::cout << (trainer->getCustomer(pair[i].first))->getName() << " is Doing "
                      << (pair[i].second.getName()) << endl;
        }
        complete();
    }
    studio.addAction(this);
}

std::string Order::toString() const {
    if (getStatus() == COMPLETED)
        return "Order" + to_string(trainerId) + " Completed";
    else
        return "Order" + to_string(trainerId) + " " + getErrorMsg();
}

BaseAction *Order::clone() const {
    return new Order(*this);
}

MoveCustomer::MoveCustomer(int src, int dst, int customerId) :
        srcTrainer(src), dstTrainer(dst), id(customerId) {}


void MoveCustomer::act(Studio &studio) {
    if (srcTrainer != dstTrainer) {
        Trainer *t_src = studio.getTrainer(srcTrainer);
        Trainer *t_dst = studio.getTrainer(dstTrainer);
        size_t t_dst_cap = t_dst->getCapacity();
        if (t_src->isOpen() && t_dst->isOpen()) {
            if (!(t_src->isOpen()) || !(t_dst->isOpen()) || (t_src == nullptr) || (t_dst == nullptr) ||
                t_dst_cap == t_dst->getCustomers().size() || t_src->getCustomer(id) == nullptr) {
                error("Cannot move customer");
            } else {
                t_dst->addCustomer(t_src->getCustomer(id));
                for (auto i: t_src->getOrders()) {
                    if (i.first == id) {
                        t_dst->getOrders().push_back(i);
                        t_dst->salary += i.second.getPrice();
                    }
                }
                t_src->removeCustomer(id);
                if (t_src->getCustomers().empty())
                    t_src->closeTrainer();
                complete();
            }
        }
    }
    studio.addAction(this);
}

std::string MoveCustomer::toString() const {
    if (getStatus() == COMPLETED) {
        return ("move" + std::to_string(dstTrainer) + std::to_string(srcTrainer) + std::to_string(id)) + " Completed";
    } else {
        return ("move" + std::to_string(dstTrainer) + std::to_string(srcTrainer) + std::to_string(id)) + " " +
               getErrorMsg();
    }
}

BaseAction *MoveCustomer::clone() const {
    return new MoveCustomer(*this);
}

Close::Close(int id) : trainerId(id) {}

void Close::act(Studio &studio) {
    Trainer *trainer = studio.getTrainer(trainerId);
    if (trainer == nullptr || !(trainer->isOpen())) {
        error("Trainer does not exist or is not open");
    } else {
        trainer->closeTrainer();
        std::cout << "Trainer " << trainerId << " closed." << "Salary " << trainer->getSalary() << "NIS" << endl;
        complete();
    }
    studio.addAction(this);
}

std::string Close::toString() const {
    if (getStatus() == COMPLETED)
        return "close" + to_string(trainerId) + " Completed";
    else
        return "close" + to_string(trainerId) + " " + getErrorMsg();

}

BaseAction *Close::clone() const {
    return new Close(*this);
}

CloseAll::CloseAll() {}

void CloseAll::act(Studio &studio) {
    if (studio.getNumOfTrainers() != 0) {
        for (int i = 0; i < studio.getNumOfTrainers(); ++i) {
            if (studio.getTrainer(i)->isOpen()) {
                std::cout << "Trainer " << i << " closed. Salary " << studio.getTrainer(i)->getSalary() << "NIS"
                          << endl;
                (studio.getTrainer(i))->closeTrainer();
            }
        }
    }
    complete();
    studio.addAction(this);
}

std::string CloseAll::toString() const {
    return "closeall Completed";
}

BaseAction *CloseAll::clone() const {
    return new CloseAll(*this);
}

PrintWorkoutOptions::PrintWorkoutOptions() {}

void PrintWorkoutOptions::act(Studio &studio) {
    for (auto i: studio.getWorkoutOptions()) {
        std::cout << i.getName() << ", " << i.getType() << ", " << i.getPrice() << std::endl;
    }
    complete();
    studio.addAction(this);
}

std::string PrintWorkoutOptions::toString() const {
    return "workout_options Completed" ;
}

BaseAction *PrintWorkoutOptions::clone() const {
    return new PrintWorkoutOptions(*this);
}

PrintTrainerStatus::PrintTrainerStatus(int id) :
        trainerId(id) {}

void PrintTrainerStatus::act(Studio &studio) {
    Trainer *trainer = studio.getTrainer(trainerId);
    if (trainer->isOpen()) {
        std::cout << "Trainer " << trainerId << " status: open" << std::endl;
        std::cout << "Customers:" << std::endl;
        for (auto i: trainer->getCustomers()) {
            std::cout << i->getId() << " " << i->getName() << endl;
        }
        std::cout << "Orders:" << std::endl;
        for (auto i: trainer->getOrders()) {
            std::cout << i.second.getName() << " " << i.second.getPrice() << "NIS " << i.first << std::endl;
        }
        std::cout << "Current Trainer's Salary: " << trainer->getSalary() << "NIS" << std::endl;
        complete();
    } else if (!(trainer->isOpen()) && trainer->getWasEverOpen()) {
        std::cout << "Trainer " << trainerId << " status: closed" << std::endl;
        complete();
    }
    studio.addAction(this);
}

std::string PrintTrainerStatus::toString() const {
    return "status " + to_string(trainerId) + " Completed";
}

BaseAction *PrintTrainerStatus::clone() const {
    return new PrintTrainerStatus(*this);
}

PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(Studio &studio) {
    for (auto i: studio.getActionsLog()) {
        if ((i->toString()).substr(0, 3) != "log")
            std::cout << i->toString() << std::endl;
    }
    complete();
    studio.addAction(this);
}

std::string PrintActionsLog::toString() const {
    return "log Completed";
}

BaseAction *PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}

BackupStudio::BackupStudio() {}

void BackupStudio::act(Studio &studio) {
    studio.updateWasBackedUp(true);
    delete backup;
    backup = new Studio(studio);
    complete();
    studio.addAction(this);

}

std::string BackupStudio::toString() const {
    return "backup Completed";
}

BaseAction *BackupStudio::clone() const {
    return new BackupStudio(*this);
}

RestoreStudio::RestoreStudio() {}

void RestoreStudio::act(Studio &studio) {
    if (studio.getWasBackedUp()) {
        studio = *backup;
        complete();
    } else {
        error("No backup available");
    }
    studio.addAction(this);
}

std::string RestoreStudio::toString() const {
    if (getStatus() == COMPLETED) {
        return "Restore Completed";
    } else {
        return "Restore " + getErrorMsg();
    }
}

BaseAction *RestoreStudio::clone() const {
    return new RestoreStudio(*this);
}