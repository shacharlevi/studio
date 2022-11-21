#include "../include/Workout.h"
#include "../include/Trainer.h"
#include "../include/Action.h"
#include "../include/Studio.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Studio::Studio() : open(), trainers(), workout_options(), actionsLog() {
    wasBackedUp = false;
}

Studio::Studio(const std::string &configFilePath) : open(false), trainers(), workout_options(), actionsLog() {
    wasBackedUp = false;
    std::ifstream input(configFilePath);
    std::vector<std::string> e;
    if (input.is_open()) {
        std::string step;
        while (std::getline(input, step)) {
            e.push_back(step);
        }
        input.close();
        std::string num;
        size_t location = 0;
        while ((location = e[4].find(',')) != std::string::npos) {
            num = e[4].substr(0, location);
            trainers.push_back(new Trainer(std::stoi(num)));
            e[4].erase(0, location + 1);
        }
        trainers.push_back(new Trainer(std::stoi(e[4])));
        for (int i = 7; i < (int) e.size() && (int) e[i].size() > 0; ++i) {
            std::string num;
            size_t location = 0;
            std::vector<std::string> eVector;
            while ((location = e[i].find(", ")) != std::string::npos) {
                num = e[i].substr(0, location);
                eVector.push_back(num);
                e[i].erase(0, location + 2);
            }
            eVector.push_back(e[i]);
            WorkoutType worktype;
            if (eVector[1] == "Cardio")
                worktype = CARDIO;
            else if (eVector[1] == "Mixed")
                worktype = MIXED;
            else
                worktype = ANAEROBIC;
            workout_options.emplace_back(i - 7, eVector[0], std::stoi(eVector[2]), worktype);
        }
    }
}

Studio::~Studio() {
    for (auto i: trainers) {
        delete i;
    }
    for (auto i: actionsLog) {
        delete i;
    }
}

//copy constructor
Studio::Studio(const Studio &studio) : open(studio.open), trainers(),
                                       workout_options(), actionsLog() {
    wasBackedUp = studio.wasBackedUp;
    for (const auto &i: studio.workout_options) {
        workout_options.push_back(i);
    }

    for (auto i: studio.actionsLog) {
        actionsLog.push_back(i->clone());
    }

    for (const Trainer *i: studio.trainers) {
        trainers.push_back(new Trainer(*i));
    }

}

//copy assigment operator
Studio &Studio::operator=(const Studio &studio) {
    if (&studio != this) {
        for (auto i: trainers) {
            delete i;
        }
        for (auto i: actionsLog) {
            delete i;

        }
    }
    std::vector<Trainer *> t;
    for (Trainer *i: studio.trainers) {
        Trainer *trainer = new Trainer(*i);
        t.push_back(trainer);
    }
    std::vector<BaseAction *> al;
    for (auto i: studio.actionsLog) {
        if (dynamic_cast<OpenTrainer *>(i)){
            al.push_back(new OpenTrainer(*dynamic_cast<OpenTrainer *>(i)));
        }
        if (dynamic_cast<Order *>(i)){
            al.push_back(new Order(*dynamic_cast<Order *>(i)));
        }
        if (dynamic_cast<MoveCustomer *>(i)){
            al.push_back(new MoveCustomer(*dynamic_cast<MoveCustomer *>(i)));
        }
        if (dynamic_cast<Close *>(i)){
            al.push_back(new Close(*dynamic_cast<Close *>(i)));
        }
        if (dynamic_cast<CloseAll *>(i)){
            al.push_back(new CloseAll(*dynamic_cast<CloseAll *>(i)));
        }
        if (dynamic_cast<PrintWorkoutOptions *>(i)){
            al.push_back(new PrintWorkoutOptions(*dynamic_cast<PrintWorkoutOptions *>(i)));
        }
        if (dynamic_cast<PrintTrainerStatus *>(i)){
            al.push_back(new PrintTrainerStatus(*dynamic_cast<PrintTrainerStatus *>(i)));
        }
        if (dynamic_cast<PrintActionsLog *>(i)){
            al.push_back(new PrintActionsLog(*dynamic_cast<PrintActionsLog *>(i)));
        }
        if (dynamic_cast<BackupStudio *>(i)){
            al.push_back(new BackupStudio(*dynamic_cast<BackupStudio *>(i)));
        }
        if (dynamic_cast<RestoreStudio *>(i)){
            al.push_back(new RestoreStudio(*dynamic_cast<RestoreStudio *>(i)));
        }
    }
    std::vector<Workout> wo;
    for (auto i: studio.workout_options)
        wo.push_back(i);
    open = studio.open;
    wasBackedUp = studio.wasBackedUp;
    trainers = t;
    actionsLog = al;
    workout_options = std::move(wo);
    return *this;

}

//move constructor
Studio::Studio(Studio &&studio) : open(studio.open), trainers(studio.trainers),
                                  workout_options(studio.workout_options), actionsLog(studio.actionsLog) {
    wasBackedUp = studio.wasBackedUp;
    studio.trainers = std::vector<Trainer *>();
    studio.actionsLog = std::vector<BaseAction *>();

}

//move assigment operator
Studio &Studio::operator=(Studio &&studio) {
    if (this != &studio) {
        for (auto i: trainers) {
            delete i;
        }
        for (auto i: actionsLog) {
            delete i;
        }
        wasBackedUp = studio.wasBackedUp;
        open = studio.open;
        trainers = studio.trainers;
        actionsLog = studio.actionsLog;
        studio.trainers = std::vector<Trainer *>();
        studio.actionsLog = std::vector<BaseAction *>();
        studio.workout_options = std::vector<Workout>();
    }
    return *this;
}

void Studio::start() {
    std::cout << "Studio is now open!" << std::endl;
    open = true;
    int id_cus = 0;
    string input;
    std::vector<Customer *> CL;
    while (open) {
        getline(cin, input);
        int posi = (input.find(" "));
        string whatA = input.substr(0, (posi));
        //OpenTrainer
        if (whatA == "open") {
            int count = 5;
            while (input.at(count) != ' ') {
                ++count;
            }
            int id = std::stoi(input.substr(5, count - 5));
            ++count;
            string name;
            string strategy;
            for (size_t i = count + 1; i < input.size(); ++i) {
                if (input.at(i) == ',' && name.empty()) {
                    name = input.substr(count, i - count);
                    count = i;
                }
                if (input.at(i == ',') && !name.empty()) {
                    strategy = input.substr(count + 1, 3);
                    i = i + 4;
                    count = i + 1;
                }
                if (!name.empty() && !strategy.empty()) {
                    if (strategy == "chp") {
                        CheapCustomer *customer = new CheapCustomer(name, id_cus);
                        CL.push_back(customer);
                    }
                    if (strategy == "mcl") {
                        HeavyMuscleCustomer *customer = new HeavyMuscleCustomer(name, id_cus);
                        CL.push_back(customer);
                    }
                    if (strategy == "fbd") {
                        FullBodyCustomer *customer = new FullBodyCustomer(name, id_cus);
                        CL.push_back(customer);
                    }
                    if (strategy == "swt") {
                        SweatyCustomer *customer = new SweatyCustomer(name, id_cus);
                        CL.push_back(customer);
                    }
                    ++id_cus;
                    name = "";
                    strategy = "";
                }
            }

            OpenTrainer *o = new OpenTrainer(id, CL);
            o->act(*this);
        }
        //order
        if (whatA == "order") {
            int pos = input.find(" ");
            Order *action = new Order(std::stoi(input.substr(pos + 1)));
            action->act(*this);
        }

        //move
        if (whatA == "move") {
            int step = 0;
            int i = 6;
            int src;
            int dst;
            int id;
            int begin;
            while (step < 3) {
                if (step == 0 && input.at(i) == ' ') {
                    src = std::stoi(input.substr(5, i - 5));
                    ++step;
                    begin = i + 1;
                    i++;
                }
                if (step == 1 && input.at(i) == ' ') {
                    dst = std::stoi(input.substr(begin, i - begin));
                    begin = i + 1;
                    ++step;
                }
                if (step == 2) {
                    id = std::stoi(input.substr(begin, input.size() - i));
                    ++step;
                }
                ++i;
            }
            MoveCustomer *cust = new MoveCustomer(src, dst, id);
            cust->act(*this);
        }

        //closeAll

        if (whatA == "closeall") {
            CloseAll *t = new CloseAll();
            t->act(*this);
            open = false;
        }
        //close

        if (whatA == "close") {
            int id = std::stoi(input.substr(6, input.size() - 6));
            Close *c = new Close(id);
            c->act(*this);
        }

        // printWorkoutOptions
        if (whatA == "workout_options") {
            PrintWorkoutOptions *t = new PrintWorkoutOptions();
            t->act(*this);
        }

        //PrintTrainerStatus
        if (whatA == "status") {
            int id = std::stoi(input.substr(7, input.size() - 7));
            PrintTrainerStatus *x = new PrintTrainerStatus(id);
            x->act(*this);
        }

        //printActionLog
        if (whatA == "log") {
            PrintActionsLog *p = new PrintActionsLog();
            p->act(*this);
        }
        //backup studio
        if (whatA == "backup") {
            BackupStudio *b = new BackupStudio();
            b->act(*this);
            //       delete b;

        }
        //RestoreStudio
        if (whatA == "restore") {
            RestoreStudio *r = new RestoreStudio();
            r->act(*this);
        }
        CL.clear();
    }

}

int Studio::getNumOfTrainers() const {
    return trainers.size();
}

Trainer *Studio::getTrainer(int tid) {
    size_t t_id = tid;
    if (t_id >= trainers.size()) {
        return nullptr;
    } else {
        return trainers[tid];
    }

}

const std::vector<BaseAction *> &Studio::getActionsLog() const {
    return actionsLog;
}

std::vector<Workout> &Studio::getWorkoutOptions() {
    return workout_options;
}

Studio *Studio::clone() const {
    return new Studio(*this);
}

void Studio::addAction(BaseAction *action) {
    actionsLog.push_back(action);
}

void Studio::updateWasBackedUp(bool input) {
    wasBackedUp = input;
}

bool Studio::getWasBackedUp() {
    return wasBackedUp;
}