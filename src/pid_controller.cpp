#include "pid_controller.h"

namespace pid_controller {




int16_t update(int32_t value, pid_data *data)
{
    //Regelabweichung
    int32_t diff = data->target - value;

    //Regler update
    int32_t update = data->kP * diff;
    if (data->kI > 0)
        update += (data->sum) * data->kI ;
    if (data->kD > 0)
    {
        update += (diff - data->last_diff) * data->kD;
        //Letzte differenz für D Anteil sichern
        data->last_diff = diff;
    }

    bool reached = false;
    update /= 16;

    //Auf maximum begrenzen
    update = ensure_max(update, &reached, data);

    //Nur wenn wir nicht in die Begrenzung reinlaufen dann I Anteil dazu zählen
    if (!reached)
        data->sum += diff;

    //Integral summe begrenzen
    if (data->kI > 0) {
        if (data->sum > data->sum_max)
            data->sum = data->sum_max;
        else if (data->sum < -data->sum_max)
            data->sum = -data->sum_max;
    }

    return update;
}

int16_t ensure_max(int32_t update, bool *reached, pid_data *data)
{
    *reached = false;
    if (update > data->maximum) {
        update = data->maximum;
        *reached = true;
    } else if (update < -data->maximum) {
        update = -data->maximum;
        *reached = true;
    }
    return update;
}
}
