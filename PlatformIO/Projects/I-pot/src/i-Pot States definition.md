i-Pot States definition

It will be able to configure different levels of interaction with the plant. 
1 . Intense
2 . Active
3 . Chill
4 . Silent

We will also setup different attitudes:
1 . Suave / Educado
2 . Revelde
3 . Picaro
4 . Maleducado

Dependiendo del nivel de intensidad y del tipo de actitud, vamos a definir la interacción del usuario con la planta.

En el primer nivel de alarma, es decir, cuando la planta este en sus condiciones optimas y no requiera de ninguna acción, es cuando solo dirá cosas relacionadas con que bien se siente y lo bien que va todo. (En el caso de tener el nivel de intensidad alto: habra que tener audios que no son importantes pero que la planta este interactuando de manera regular.)

En el segundo nivel de alarma (Amarillo), la planta pondrá emfasis en esa condición que requiere intervención. Es decir, si la planta neceista agua, la planta irá diciendo al usuario que necesita de su intervención. En el caso de tener puesto el nivel de intensidad en un nivel bajo de intensidad o incluso en silencio, al entrar en este nivel de alarma se ignorará el silencio y la planta va a intervenir.

En el tercer nivel de alarma (Rojo) la planta estará en peligro, con lo qual no dejará de avisar y de intervenir diciendole al usuario los requerimientos que precisa y explicarle que está en peligro su vida. En este nivel, cualquier tipo de intensidad en la planta baja será ignorado con tal de poder transmitir al usuario todo lo que ella necesita.

---

1. Initialization
2. Components and sensors failure check.
3. Sensor readings.
4. If sensor value out of range, definition of level.
5. If everything okay, normal use.
6. If anything is out of the normal range, start request to user.
7. If user solved problem, plant level green. 
8. Else, continue requesting to user.
9. In case of getting to high level of danger, request to user with intensity. (Possibility to send an SMS to user)
10. In case it gets solved, go back to yellow level.

___

-- Configuration Portal --

In this portal we will be able to configure the i-pot. 

1 . Level of intensity
2 . Attitude
3 . Wifi credentials
4 . I-pot Name
5 . ??











---


Depend on the level of warning we act different.

1. Gren level --  
This is the normal state of the plant. Where everything is okay and the plant just continues 
reading data from the sensors. 

At this point we 

2. Yellow level -- In this statte the plant is needing something or the sensors are reading a value out of the
range.

3. Red level -- At that point, the plant is in danger or it has many needs to be covered. The value of the
sensors are in the danger range.


