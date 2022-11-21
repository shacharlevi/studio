#include <vector>
#include <string>
#include "../include/Workout.h"
#include "../include/Customer.h"
#include <iostream>
#include <algorithm>
#include <tuple>

Customer::Customer(std::string
                   c_name,
                   int c_id
) :

        name(c_name), id(c_id) {}

std::string Customer::getName() const {
    return name;
}

int Customer::getId() const {
    return id;
}


SweatyCustomer::SweatyCustomer(std::string name, int id) :
        Customer(name, id) {}

std::vector<int> SweatyCustomer::order(const std::vector<Workout> &workout_options) {
    std::vector<int> myVector;
    for (size_t i = 0; i < workout_options.size(); i = i + 1) {
        if ((workout_options[i]).getType() == CARDIO) {
            myVector.push_back((workout_options[i]).getId());
        }
    }
    return myVector;
}

std::string SweatyCustomer::toString() const {
    return "swt";
}

Customer *SweatyCustomer::clone() const {
    return new SweatyCustomer(*this);
}


CheapCustomer::CheapCustomer(std::string name, int id) :
        Customer(name, id) {}


std::vector<int> CheapCustomer::order(const std::vector<Workout> &workout_options) {
    std::vector<int> myVector;
    int t = 0;
    for (size_t i = 1; i < workout_options.size(); ++i) {
        if ((workout_options[t]).getPrice() > workout_options[i].getPrice()) {
            t = i;
        }
    }
    myVector.push_back(workout_options[t].getId());
    return myVector;
}

std::string CheapCustomer::toString() const {
    return "chp";
}

Customer *CheapCustomer::clone() const {
    return new CheapCustomer(*this);
}

HeavyMuscleCustomer::HeavyMuscleCustomer(std::string name, int id) :
        Customer(name, id) {}


std::vector<int> HeavyMuscleCustomer::order(const std::vector<Workout> &workout_options) {
    std::vector<int> ordering;
    for (const auto &wo: workout_options) {
        if (wo.getType() == ANAEROBIC) {
            ordering.push_back(wo.getId());
        }
    }
    std::stable_sort(ordering.begin(), ordering.end(), [&workout_options](int i1, int i2) {
        return (workout_options[i1].getPrice() > workout_options[i2].getPrice());
    });
    return ordering;
}


std::string HeavyMuscleCustomer::toString() const {
    return "mcl";
}

Customer *HeavyMuscleCustomer::clone() const {
    return new HeavyMuscleCustomer(*this);
}

FullBodyCustomer::FullBodyCustomer(std::string name, int id) :
        Customer(name, id) {}


std::vector<int> FullBodyCustomer::order(const std::vector<Workout> &workout_options) {
    int cheap_car = -1;
    int exp_mixed = -1;
    int cheap_an = -1;
    for (size_t i = 0; i < workout_options.size(); ++i) {
        if (workout_options[i].getType() == CARDIO &&
            (cheap_car == -1 || workout_options[i].getPrice() < workout_options[cheap_car].getPrice())) {
            cheap_car = i;
        }
        if (workout_options[i].getType() == MIXED &&
            (exp_mixed == -1 || workout_options[i].getPrice() > workout_options[exp_mixed].getPrice())) {
            exp_mixed = i;
        }
        if (workout_options[i].getType() == ANAEROBIC &&
            (cheap_an == -1 || workout_options[i].getPrice() < workout_options[cheap_an].getPrice())) {
            cheap_an = i;
        }
    }
    std::vector<int> retVector = {cheap_car, exp_mixed, cheap_an};
    return retVector;
}


std::string FullBodyCustomer::toString() const {
    return "fbd";
}

Customer *FullBodyCustomer::clone() const {
    return new FullBodyCustomer(*this);
}