### Korrekturfaktor
Mit hilfe des Korrekturfaktors können, wenn benötigt, die Toleranzen des Messsystems ausgeglichen werden.

Um die 'vom Device gemessene Spannung' zu bestimmen, muss zuvor der Sensortyp auf 'ADC-Wert(mV)' und die Meßwerteinheit auf 'Spannung (mV)' gestellt werden. Dazu ein passendes KO belegt werden, welches den Wert ausgibt.

Korrekturfaktor = gemessene Spannung(mV) am Device-Pin(Kabelverschraubung am Gerät) / vom Device ausgegebene Spannung(mV)

Hinweis: bei 4-20mA entsprechen 4mA = 400mV und 20mA = 2V (gemessen am Device-Pin)

Danach kann der Korrekturfaktor bestimmt und eingetragen werden und der Sensortyp und die Meßwerteinheit auf die gewünschte Einheit gestellt werden.

### Meßwerteinheit (KO)
Hier kann die gewünschte Einheit/DPT ausgewählt werden. Bein manchen Einheiten gibt es weitere Einstellmöglichgkeiten mit denen man die Kennlinie des angeschlossenen Sensors nachbilden kann.  

Hier kann man den jeweiligen Wert des Sensor angeben: 

#### Wert bei 4mA 

#### Wert bei 20mA 

### Zyklisch senden
Hier kann die Zeit angegeben werden, nach der das letzte gesendete Signal erneut gesendet werden soll. Dieser Vorgang wird ohne Unterbrechung wiederholt.

### senden bei absoluter Abweichung
Hier kann der Wert angegeben werden, bei welcher Wertänderung gesendet werden soll

### senden bei relativer Abweichung
Hier kann der Prozentwert angegeben werden, bei welcher Abweichung gesendet werden soll.



