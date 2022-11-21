#ifndef STUDIO_H_
#define STUDIO_H_

#include <vector>
#include <string>
#include "Workout.h"
#include "Trainer.h"
#include "Action.h"


class Studio {
public:

    Studio();

    Studio(const std::string &configFilePath);

    virtual ~Studio();

    Studio(const Studio &studio);

    Studio &operator=(const Studio &studio);

    Studio(Studio &&studio);

    Studio &operator=(Studio &&studio);


    void start();

    int getNumOfTrainers() const;

    Trainer *getTrainer(int tid);

    const std::vector<BaseAction *> &getActionsLog() const; // Return a reference to the history of actions
    std::vector<Workout> &getWorkoutOptions();

    Studio *clone() const;

    void addAction(BaseAction *action);

    void updateWasBackedUp(bool input);

    bool getWasBackedUp();

private:
    bool open;
    std::vector<Trainer *> trainers;
    std::vector<Workout> workout_options;
    std::vector<BaseAction *> actionsLog;
    bool wasBackedUp;
};

#endif