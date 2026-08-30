### Korrekturfaktor
Mit hilfe des Korrekturfaktors können, wenn benötigt, die Toleranzen des Messsystems ausgeglichen werden.

Um die 'vom Device gemessene Spannung' zu bestimmen, muss zuvor der Sensortyp auf 'ADC-Wert(mV)' und die Meßwerteinheit auf 'Spannung (mV)' gestellt werden. Dazu ein passendes KO belegt werden, welches den Wert ausgibt.

Danach kann der Korrekturfaktor bestimmt und eingetragen werden und der Sensortyp und die Meßwerteinheit auf die gewünschte Einheit gestellt werden.

Korrekturfaktor = gemessene Spannung(mV) am Device-Pin / vom Device ausgegebene Spannung(mV)

### Meßwerteinheit (KO)
Hier kann die gewünschte Einheit/DPT ausgewählt werden. Bein manchen Einheiten gibt es weitere Einstellmöglichgkeiten mit denen man die Kennlinie des angeschlossenen Sensors nachbilden kann.  

Der Sensorwert wird über die Geradengleichung y = m*x + b bestimmt. Ist die Geradengleichung des Sensors nicht bekannt, kann diese über zwei Punkte auf der Kennlinie (x,y) bestimmt werden. 
Hilfe: bei Google nach "Geradengleichung aus 2 punkten rechner" suchen. Anschließend findet man einfach rechner, wo man die zwei Punkte nur noch eingeben muss. 

#### Wert m
Steigung der Gerade

#### Wert b
Offset der Gerade

### Zyklisch senden
Hier kann die Zeit angegeben werden, nach der das letzte gesendete Signal erneut gesendet werden soll. Dieser Vorgang wird ohne Unterbrechung wiederholt.

### senden bei absoluter Abweichung
Hier kann der Wert angegeben werden, bei welcher Wertänderung gesendet werden soll

### senden bei relativer Abweichung
Hier kann der Prozentwert angegeben werden, bei welcher Abweichung gesendet werden soll.



