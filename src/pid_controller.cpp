#include "pid_controller.h"

PidController::PidController():sum(0),last_diff(0),target(0){

}

int16_t PidController::update(int32_t value)
{
    //Regelabweichung
    int32_t diff = target - value;

    //Regler update
    int32_t update = kP * diff;
    if (kI != 0)
        update += sum * kI ;
    if (kD != 0)
    {
        update += (diff - last_diff) * kD;
        //Letzte differenz für D Anteil sichern
        last_diff = diff;
    }

    bool reached = false;
    update /= 16;

    //Auf maximum begrenzen
    if (update > maximum) {
        update = maximum;
        reached = true;
    } else if (update < -maximum) {
        update = -maximum;
        reached = true;
    }

    //Nur wenn wir nicht in die Begrenzung reinlaufen dann I Anteil dazu zählen
    if (!reached)
        sum += diff;

    //Integral summe begrenzen
    if (kI != 0) {
        if (sum > sum_max)
            sum = sum_max;
        else if (sum < -sum_max)
            sum = -sum_max;
    }
    return update;
}
