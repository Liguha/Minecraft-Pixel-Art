# Minecraft Pixel Art Generator
## Описание
Программа для генерации пиксель арта для игры Minecraft по загруженному изображению. В качестве результата генерируется последовательность команд "/setblock". 
Для расширения палитры программа строит многослойные арты (цветное стекло на верхних слоях), количество слоёв является настраивамым параметром.  
Программа работает на Windows и использует библиотеку OpenCV. 
## Примеры
В качестве изображения была передана следующая картинка:  
<img src="./assets/original.jpg" alt="original" height="400"/>  
Полученный двуслойный арт:  
<img src="./assets/2layers.png" alt="2 layers" height="400"/>  
Полученный трёхслойный арт:  
<img src="./assets/3layers.png" alt="3 layers" height="400"/>  
