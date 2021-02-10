/*
Если Вы хотите запустить программу, откройте Microsoft Visual Studio, создайте пустой проект С++, подключите SFML
(как это сделать, можно посмотреть по ссылке https://kychka-pc.ru/sfml/urok-1-podklyuchenie-biblioteki-k-srede-razrabotki-visual-studio-2013.html - статья;
или по ссылке https://www.youtube.com/watch?v=w339OWGlSo0 - видео).

Либо вы можете запустить ".exe" файл, если он у Вас есть. К нему прилагаются необходимые ".dll" файлы и каталог с ресурсами программы.

Ниже приведён код программы с подробными комментариями.
*/

#include <SFML/Graphics.hpp> // заголовочные файлы
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

using namespace sf; // используем по умолчанию пространство имён sf библиотеки SFML

const int width = 800, height = 600, rect = 40, stone_fly = 800; // размеры окна, размер блока, дальность полёта камня
const double game_speed = 2250, fps = 0.03, hero_speed = 0.7, acs = 0.005; // константы для скоростей и ускорений
bool game_over = false, show_menu = true, to_menu = false, to_menu_from_restart_menu; // окончена ли игра, показывается ли сейчас меню, нужно ли вернуться в меню
int gold, Health, level = 0, stones = 3; // монеты, максимальное здоровье, выбранный уровень, текущее количество камней
bool buyed[4] = { false, false, false, false }, the_end = false, no_stones = false, was_conflict = false, awarded = false, new_game = false, restart = false, pause = false; // куплен ли товар в магазине, произошёл ли конец игры, есть ли камни, был ли конфликт в игрока с чем-либо в предыдущий момент времени, были ли начислены монеты за оставшиеся камни, нужно ли начать новую игру, хочет ли игрок переиграть уровень
double player_rect[4]; // текущие координаты и размеры игрока, нужны чтобы обрабатывать столкновения

const int map_h[10] = { 33, 33, 33, 0, 0, 0, 0, 0, 0, 0 }; // массивы с размерами каждого уровня
const int map_w[10] = { 54, 54, 54, 0, 0, 0, 0, 0, 0, 0 };
const int start_pos[10][2] = { {rect, 16 * rect}, {rect, 27 * rect}, {rect, 16 * rect}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} }; // стартовая позиция игрока в каждом уровне
const int enemy_count[10] = { 4, 5, 0, 0, 0, 0, 0, 0, 0, 0 }; // количества врагов в каждом уровне

const int frames[8][4] = { {20, 10, 90, 125}, {125, 10, 90, 125}, {215, 10, 120, 125}, {335, 10, 95, 125}, {450, 10, 80, 125}, {550, 10, 90, 125}, {650, 10, 100, 125}, {770, 10, 90, 125} }; // текстуры персонажа
const int skelet[11][4] = { {40 * 1.3, 23 * 1.3, 68 * 1.3, 119 * 1.3}, {128 * 1.3, 25 * 1.3, 63 * 1.3, 119 * 1.3}, {215 * 1.3, 23 * 1.3, 55 * 1.3, 119 * 1.3}, {290 * 1.3, 21 * 1.3, 48 * 1.3, 119 * 1.3}, {358 * 1.3, 23 * 1.3, 52 * 1.3, 119 * 1.3}, {429 * 1.3, 23 * 1.3, 68 * 1.3, 119 * 1.3}, {518 * 1.3, 23 * 1.3, 67 * 1.3, 119 * 1.3}, {605 * 1.3, 23 * 1.3, 69 * 1.3, 119 * 1.3}, {694 * 1.3, 23 * 1.3, 53 * 1.3, 119 * 1.3}, {768 * 1.3, 23 * 1.3, 63 * 1.3, 119 * 1.3}, {849 * 1.3, 23 * 1.3, 65 * 1.3, 119 * 1.3} }; // текстуры врагов
const int main_menu[6][4] = { {207, 95, 188, 57}, {207, 164, 188, 57}, {207, 231, 188, 57}, {207, 301, 188, 57}, {207, 372, 188, 57}, {206, 448, 198, 63} }; // текстуры главного меню
const int shop[9][4] = { {397, 28, 360, 56}, {397, 97, 360, 56}, {397, 163, 360, 56}, {397, 230, 360, 56}, {207, 372, 188, 57}, {440, 488, 337, 122}, {440, 630, 339, 75}, {398, 301, 367, 66}, {206, 448, 198, 63} }; // текстуры магазина
const int shop_pos[7][2] = { {20, 200}, {20, 300}, {20, 400}, {20, 500}, {width - 218, height - 83}, {20, 20}, {width - 359, 20} }; // координаты объектов в магазине
const int change[13][4] = { {891, 267, 251, 68}, {891, 343, 251, 68}, {891, 417, 251, 68}, {891, 492, 251, 68}, {891, 567, 251, 68}, {891, 646, 251, 68}, {891, 726, 251, 68}, {1151, 269, 251, 68}, {1151, 342, 251, 68}, {1151, 416, 251, 68}, {1152, 492, 251, 68}, {1403, 269, 68, 62}, {1149, 569, 262, 77} }; // текстуры для меню уровней
const int change_pos[11][2] = { {50, 20}, {50, 120}, {50, 220}, {50, 320}, {50, 420}, {width - 400, 20}, {width - 400, 120}, {width - 400, 220}, {width - 400, 320}, {width - 400, 420}, {width - 270, height - 90} }; // координаты объектов для меню уровней
const int about_game[3][4] = { {207, 372, 188, 57}, {830, 70, 530, 175}, {206, 448, 198, 63 } }; // текстуры для меню "об игре"
const int about_game_pos[3][2] = { {width - 208, height - 77}, {200, 200}, {width - 211, height - 80} }; // координаты объектов для меню "об игре"
const int price[8] = { 0, 0, 0, 0, 10, 30, 60, 100 }; // цены в магазине

String int_to_str(int n) // функция для преобразования числа в строку, поможет в дальнейшем для вывода чисел в тексте
{
	String ans = "", tmp = "";
	int n1 = n;
	while (n1 >= 10)
	{
		tmp += char(n1 % 10 + 48);
		n1 /= 10;
	}
	tmp += char(n1 % 10 + 48);
	for (int i = tmp.getSize() - 1; i >= 0; --i)
		ans += tmp[i];
	return ans;
}

String m_map[10][33] = { // уровни, ПОКА ЕСТЬ ТОЛЬКО 2, ЧТОБЫ СОЗДАТЬ НОВЫЙ, НАДО ПРОСТО ИЗМЕНИТЬ СООТВЕТСТВУЮЩИЕ МАССИВ	m_map[level], дописать параметры игроков в Menu::update_level() и поменять размеры уровня в самом начале программы
{"                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "   h  m  m                         m                  ",
 "                     13                               ",
 "12222222222222223    79      m    122223              ",
 "78888888888888889                 455556              ",
 "                            13    4555552223          ",
 "                       h    46    4555555556          ",
 "                            46    45555555552223      ",
 "                      13    46    45588888888889      ",
 "       m              46    46    456                 ",
 "                      46    46    456                 ",
 "1222222223    12222222552222552222556  hm             ",
 "4555555556    45555555555555555555556                 ",
 "4555555556    45555555555555555555555222222222223     ",
 "4555555556    45888888888888888888888888888888889     ",
 "4555555555222256                                      ",
 "4555555555555556                                      ",
 "4555555555555556                                      ",
 "4555555555555556                                      ",
 "4555555555555556f    m         m  h            h      ",
 "4555555555555556                                      ",
 "455555555555555522222222222222222222222222222222222223",
 "788888888888888888888888888888888888888888888888888889"}, // уровень 1

{"                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                   f  ",
 "                                                 mm   ",
 "                             13       1222222222222223",
 "                             79       7888888888888889",
 "       m               h                              ",
 "      13       122222223                              ",
 "      79       788888889                              ",
 "                                                      ",
 "13                                                    ",
 "79                                                    ",
 "         h                                            ",
 "   13    12222222223                                  ",
 "   79    78888888889    12223                         ",
 "                        78889     mh                  ",
 "                                  1222222222223       ",
 "                                  7888888888889       ",
 "                                                     h",
 "                                                    13",
 "                                                    46",
 "                                     m              46",
 "                                    13        12222256",
 "                   m                79        78888889",
 "                  13        13           13           ",
 "         13       79        79           79           ",
 "         79                                           ",
 "                                                      ",
 "122222222222222222222222222222222222222222222222222223",
 "455555555555555555555555555555555555555555555555555556",
 "788888888888888888888888888888888888888888888888888889"}, // уровень 2

{"                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "                                                      ",
 "            5         5                  5            ",
 "            5         5         5        5          f ",
 "    5       5         5         5        5            ",
 "1222222222223      12222222222222222222222222222222223",
 "4555555555556      45555555555555555555555555555555556",
 "4555555555556      45555555555555555555555555555555556",
 "4555555555556      45555555555555555555555555555555556",
 "4555555555556      45555555555555555555555555555555556",
 "4555555555556      45555555555555555555555555555555556",
 "4555555555556      45555555555555555555555555555555556",
 "4555555555556      45555555555555555555555555555555556",
 "4555555555556      45555555555555555555555555555555556",
 "455555555555522222255555555555555555555555555555555556",
 "455555555555555555555555555555555555555555555555555556",
 "455555555555555555555555555555555555555555555555555556",
 "455555555555555555555555555555555555555555555555555556",
 "788888888888888888888888888888888888888888888888888889"}, // уровень 3

{"                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                1223              ",
 "                                                  ",
 "                                                  ",
 "     h   m               h     h              m   ",
 "                                                  ",
 "1222222222223      1222222222222222222222222222223",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556  m   4555555555555555555555555555556",
 "45555555555552222225555555555555555555555555555556",
 "78888888888888888888888888888888888888888888888889",
  "",
 "",
 "" }, // уровень 4

{"                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                1223              ",
 "                                                  ",
 "                                                  ",
 "     h   m               h     h              m   ",
 "                                                  ",
 "1222222222223      1222222222222222222222222222223",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556  m   4555555555555555555555555555556",
 "45555555555552222225555555555555555555555555555556",
 "78888888888888888888888888888888888888888888888889",
 "",
 "",
 "" }, // уровень 5

{"                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                1223              ",
 "                                                  ",
 "                                                  ",
 "     h   m               h     h              m   ",
 "                                                  ",
 "1222222222223      1222222222222222222222222222223",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556  m   4555555555555555555555555555556",
 "45555555555552222225555555555555555555555555555556",
 "78888888888888888888888888888888888888888888888889",
 "",
 "",
 "" }, // уровень 6

{"                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                1223              ",
 "                                                  ",
 "                                                  ",
 "     h   m               h     h              m   ",
 "                                                  ",
 "1222222222223      1222222222222222222222222222223",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556  m   4555555555555555555555555555556",
 "45555555555552222225555555555555555555555555555556",
 "78888888888888888888888888888888888888888888888889",
 "",
 "",
 "" }, // уровень 7

{ "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                1223              ",
  "                                                  ",
  "                                                  ",
  "     h   m               h     h              m   ",
  "                                                  ",
  "1222222222223      1222222222222222222222222222223",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556  m   4555555555555555555555555555556",
  "45555555555552222225555555555555555555555555555556",
  "78888888888888888888888888888888888888888888888889",
 "",
 "",
 "" }, // уровень 8

{ "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                                  ",
  "                                1223              ",
  "                                                  ",
  "                                                  ",
  "     h   m               h     h              m   ",
  "                                                  ",
  "1222222222223      1222222222222222222222222222223",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556      4555555555555555555555555555556",
  "4555555555556  m   4555555555555555555555555555556",
  "45555555555552222225555555555555555555555555555556",
  "78888888888888888888888888888888888888888888888889",
 "",
 "",
 "" }, // уровень 9

{"                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                                  ",
 "                                1223              ",
 "                                                  ",
 "                                                  ",
 "     h   m               h     h              m   ",
 "                                                  ",
 "1222222222223      1222222222222222222222222222223",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556      4555555555555555555555555555556",
 "4555555555556  m   4555555555555555555555555555556",
 "45555555555552222225555555555555555555555555555556",
 "78888888888888888888888888888888888888888888888889",
 "                                                  ",
 "                                                  ",
 "                                                  " } // уровень 10
};

class Enemy // класс врага
{
public:
	int was_health, health, left, right, dir; // начальное здоровье, текущее здоровье, границы передвижения, направление
	double x, y, w, h, speed_w, curr; // координаты, размеры, скорость, текущий кадр
	bool life, stop, gave_stones; // жив ли, должен ли стоять, получены ли за него камни
	Image image; Texture texture; Sprite sprite; // картинка, текстура, спрайт
	Enemy(int H, int L, int R, double Y) // конструктор
	{
		was_health = H, health = H; left = L; right = R; dir = 1;
		x = left; y = Y; w = skelet[0][2]; h = skelet[0][3]; speed_w = 0; curr = 0;
		life = true; stop = false; gave_stones = false;
		image.loadFromFile("images/enemy.png");
		image.createMaskFromColor(Color(66, 66, 66)); // убираем лишний цвет с картинки
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(skelet[int(curr)][0], skelet[int(curr)][1], w, h));
	}
	bool conflict_right() // метод обработки конфликта справа
	{
		double px = player_rect[0], py = player_rect[1], pw = player_rect[2], ph = player_rect[3]; // взятие координат и размеров игрока
		char c; // пустой символ
		for (int i = 1; i < h; ++i) // смотрим по всей высоте спрайта
		{
			c = m_map[level][int(y + i) / rect][int(x + w) / rect]; //  в символ записываем то, что справа
			if (c >= 49 && c <= 57) // если справа земля, то
				return true; // конфликт есть
		}
		if (px < x + w && px > x + w / 2 && (py > y&& py < y + h || py + ph > y&& py + ph < y + h)) // если справа игрок, то
			return true; // конфликт есть
		return  false; // иначе нет конфликта
	}
	bool conflict_left() // ЭТОТ И ДАЛЬНЕЙШИЕ МЕТОДЫ ОБРАБОТКИ КОНЛИКТОВ АНАЛОГИЧНЫ И ИДУТ БЕЗ ПОЯСНЕНИЙ
	{
		double px = player_rect[0], py = player_rect[1], pw = player_rect[2], ph = player_rect[3];
		char c;
		for (int i = 1; i < h; ++i)
		{
			c = m_map[level][int(y + i) / rect][int(x - 1) / rect];
			if (c >= 49 && c <= 57)
				return true;
		}
		if (px + pw > x&& px + pw < x + w / 2 && (py > y && py < y + h || py + ph > y && py + ph < y + h))
			return true;
		return  false;
	}
	void death() // метод для обработки смерти
	{
		if (health <= 0) // если здоровья нет, то
		{
			life = false; // жизнь закончена
			if (gave_stones == false) // если камни ещё не отданы, то
			{
				stones += was_health; // отдать камни
				gave_stones = true; // и запомнить это
			}
		}
	}
	void update(double time) // метод обновления объекта
	{
		if (x + speed_w * time > 0 && x + speed_w * time < map_w[level] * rect - w) // если после перемещения объект не попадает за границы карты, то
			x += speed_w * time; // совершить перемещение
		w = skelet[int(curr)][2]; // обновление ширины и высоты объекта в соответствие с текущим кадром
		h = skelet[int(curr)][3];
		speed_w = 0; // обновление скорости (потом, если будет надо, в методе move она появится)
		death(); // проверка смерти
		sprite.setPosition(x, y); // отображение перемещения на экране
	}
	void move(double time) // метод движения объекта
	{
		if (life && !stop) // если он жив и ему можно двигаться, то
		{
			curr += fps * time * 0.7; // добавить к текущему кадру небольшую величину (не единицу, чтобы не перегружать игру, этих кадров в секунду хватит)
			if (curr > 11)
				curr -= 11;
			if (player_rect[0] + player_rect[2] > left&& player_rect[0] < right && abs(y + h / 2 - player_rect[1] - player_rect[3] / 2) < 50)
				if (player_rect[0] + player_rect[2] / 2 < x)
					dir = -1;
				else
					dir = 1;
			else if (x < left || x > right || conflict_right() || conflict_left()) // если он дошёл до границы или препятствия, то
				dir *= -1; // развернуться
			speed_w = dir * hero_speed / 2; // задание скорости
			if (dir == -1 && !conflict_left()) // переключение кадров
				sprite.setTextureRect(IntRect(skelet[int(curr)][0], skelet[int(curr)][1], skelet[int(curr)][2], skelet[int(curr)][3]));
			if (dir == 1 && !conflict_right())
				sprite.setTextureRect(IntRect(skelet[int(curr)][0] + skelet[int(curr)][2], skelet[int(curr)][1], -skelet[int(curr)][2], skelet[int(curr)][3]));
		}
	}
};

std::vector<Enemy*> enemy(enemy_count[level]); // массив врагов, он заполнится при выборе или смене уровня

class Player // класс игрока
{
public:
	int health, dir, conflicts; // здоровье, направление движения
	double x, y, w, h, speed_w, speed_h, curr, conflict_time, start_time; // координаты, размеры, скорости, кадр, текущее время конфликта и время начала конфликта
	bool jump, life, confirm_restart, close_restart_menu, was_conflict, conflict; // можно ли прыгать, живой ли, был ли конфликт с врагом при прошлой проверке и есть ли он сейчас
	Image image, i, overi, fi, si; Texture texture, t, overt, ti, st; Sprite sprite, over, s, congratulations, ss; View view; Clock clock; // картинки, текстуры и спрайты игрока, сердечка, конца игры при смерти, поздравлений с победой и камня соответственно, а также камера и часы
	Font font; Text text;
	Player(double X, double Y) // конструктор
	{
		health = Health; dir = 1; conflicts = 0;
		x = X; y = Y; w = frames[0][2]; h = frames[0][3]; speed_w = 0; speed_h = 0; curr = 0;
		jump = true; life = true; confirm_restart = false; was_conflict = false; conflict = false;

		image.loadFromFile("images/hero.png"); // игрок
		image.createMaskFromColor(Color(255, 255, 255));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(frames[int(curr)][0], frames[int(curr)][1], w, h));

		view.reset(FloatRect(0, 0, width, height)); // камера
		view.setCenter(width / 2, height / 2);

		i.loadFromFile("images/map.jpg"); // сердечки
		i.createMaskFromColor(Color(222, 220, 197));
		t.loadFromImage(i);
		s.setTexture(t);
		s.setTextureRect(IntRect(130, 240, rect, rect));

		overi.loadFromFile("images/game_over.png"); // конец игры при смерти
		overi.createMaskFromColor(Color(35, 31, 32));
		overt.loadFromImage(overi);
		over.setTexture(overt);
		over.setTextureRect(IntRect(20, 60, 190, 110));

		fi.loadFromFile("images/finish.png"); // поздравления с победой
		fi.createMaskFromColor(Color::White);
		ti.loadFromImage(fi);
		congratulations.setTexture(ti);
		congratulations.setTextureRect(IntRect(209, 76, 637 - 209, 359 - 76));

		si.loadFromFile("images/stone.jpg"); // камень
		si.createMaskFromColor(Color::White);
		si.createMaskFromColor(Color(221, 221, 221));
		st.loadFromImage(si);
		ss.setTexture(st);
		ss.setTextureRect(IntRect(5, 8, 36, 33));

		font.loadFromFile("images/CyrilicOld.TTF");
		text = Text("Начать заново? Да - Y, Нет - N\n  Выйти в главное меню - X ", font, 40);
		text.setFillColor(Color::Red);

		conflict_time = 0; start_time = 0;
	}
	void reset(double X, double Y) // обновление параметров игрока для перезапуска уровня
	{
		health = Health; dir = 1;
		x = X; y = Y; w = frames[0][2]; h = frames[0][3]; speed_w = 0; speed_h = 0; curr = 0;
		jump = true, life = true;
		conflict_time = 0; start_time = 0;
	}
	void restart_game(double X, double Y, RenderWindow& window) // перезапуск уровня
	{
		if (Keyboard::isKeyPressed(Keyboard::Escape)) // если нажата клавиша ESCAPE
		{
			restart = true; // надо открыть меню перезапуска
			pause = true; // и поставить игру на паузу
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape) == false && restart == true) // если требуется перезапуск
		{
			window.clear(Color(219, 221, 202)); // делаем чистый фон
			window.draw(text); // спрашиваем, точно ли надо перезапустить
			if (Keyboard::isKeyPressed(Keyboard::Y)) // если нажата клавиша Y
				confirm_restart = true; // перезапуск подтверждён
			if (Keyboard::isKeyPressed(Keyboard::N)) // если нажата клавиша N
				close_restart_menu = true; // отменить перезапуск, вернуться в игру
			if (Keyboard::isKeyPressed(Keyboard::X)) // если нажата клавиша X
				to_menu_from_restart_menu = true; // выйти в главное меню
			if (Keyboard::isKeyPressed(Keyboard::Y) == false && confirm_restart == true) // если перезапуск подтверждён
			{
				new_game = true; // запустить новую игру
				restart = false; // запомнить, что уже запустили
				pause = false; // убрать паузу
				confirm_restart = false; // убрать подтверждение рестарта
			}
			if (Keyboard::isKeyPressed(Keyboard::N) == false && close_restart_menu == true) // если перезапуск не подтверждён
			{
				restart = false; // забыть про рестарт
				pause = false; // убрать паузу
				close_restart_menu = false; // забыть о том, что произошло
			}
			if (Keyboard::isKeyPressed(Keyboard::X) == false && to_menu_from_restart_menu == true) // если надо выйти в гланое меню
			{
				show_menu = true; // показываем главное меню
				restart = false; // отключаем рестарт
				pause = false; // снимаемся с паузы
			}
		}
	}
	bool conflict_right() // методы конфликтов разъясняются в классе Enemy
	{
		int n = enemy_count[level];
		char c;
		for (int i = 1; i < h; ++i)
		{
			c = m_map[level][int(y + i) / rect][int(x + w) / rect];
			if (c >= 49 && c <= 57)
				return true;
		}
		for (int i = 0; i < n; ++i)
			if (enemy[i]->life == true && x + w > enemy[i]->x&& x + w < enemy[i]->x + enemy[i]->w / 2 && (y > enemy[i]->y && y < enemy[i]->y + enemy[i]->h || y + h > enemy[i]->y && y + h < enemy[i]->y + enemy[i]->h))
			{
				enemy[i]->stop = true;
				if (was_conflict == false) // если при прошлой проверке не было конфликта с врагом, и он появился
				{
					start_time = clock.getElapsedTime().asMilliseconds(); // тогда начинаем считать время конфликта
					was_conflict = true; // и отмечаем это
				}
				return true;
			}
			else
			{
				enemy[i]->stop = false;
				if (was_conflict == true) // если при прошлой проверке конфликт был, а сейчас его нет
				{
					conflict_time = clock.getElapsedTime().asMilliseconds() - start_time; // то находим время конфликта
					was_conflict = false; // отмечаем это
					if (conflict_time > 100) // если время конфликта больше 0.1 секунды
						health--; // уменьшаем здоровье игрока на 1 единицу
				}
			}
		return  false;
	}
	bool conflict_left() // методы конфликтов разъясняются в классе Enemy
	{
		int n = enemy_count[level];
		char c;
		for (int i = 1; i < h; ++i)
		{
			c = m_map[level][int(y + i) / rect][int(x - 1) / rect];
			if (c >= 49 && c <= 57)
				return true;
		}
		for (int i = 0; i < n; ++i)
			if (enemy[i]->life == true && x < enemy[i]->x + enemy[i]->w && x > enemy[i]->x + enemy[i]->w / 2 && (y > enemy[i]->y&& y < enemy[i]->y + enemy[i]->h || y + h > enemy[i]->y&& y + h < enemy[i]->y + enemy[i]->h))
			{
				enemy[i]->stop = true;
				if (was_conflict == false)
				{
					start_time = clock.getElapsedTime().asMilliseconds();
					was_conflict = true;
				}
				return true;
			}
			else
			{
				enemy[i]->stop = false;
				if (was_conflict == true)
				{
					conflict_time = clock.getElapsedTime().asMilliseconds() - start_time;
					was_conflict = false;
					if (conflict_time > 100)
						health--;
				}
			}
		return  false;
	}
	bool conflict_down() // методы конфликтов разъясняются в классе Enemy
	{
		char c;
		for (int i = 1; i < w; ++i)
		{
			c = m_map[level][int(y + h) / rect][int(x + i) / rect];
			if (c >= 49 && c <= 57)
				return true;
		}
		return false;
	}
	bool conflict_up() // методы конфликтов разъясняются в классе Enemy
	{
		char c;
		for (int i = 1; i < w; ++i)
		{
			c = m_map[level][int(y) / rect][int(x + i) / rect];
			if (c >= 49 && c <= 57)
				return true;
		}
		return  false;
	}
	void heal() // метод для лечения путём сбора сердечек и монет на карте
	{
		char c;
		for (int i = 10; i < h - 10; i += 5) // осматриваем границы спрайта игрока
			for (int j = 10; j < w - 10; j += 5)
			{
				c = m_map[level][int(y + i) / rect][int(x + j) / rect];
				if (c == 'h') // если пересечение с сердечком
				{
					if (health < Health) // если здоровья не хватает
						health++; // лечимся
					m_map[level][int(y + i) / rect][int(x + j) / rect] = ' '; // удаляем сердечко
					return;
				}
				else if (c == 'm') // если пересечение с монеткой
				{
					gold++; // собираем её
					m_map[level][int(y + i) / rect][int(x + j) / rect] = ' '; // и удаляем с карты
					return;
				}
				else if (c == 'f') // если пересечение с табличкой "финиш"
				{
					the_end = true; // конец игры и последующие поздравления
					return;
				}

			}
	}
	void death() // метод для распознавания смерти
	{
		if (health <= 0) // если нет здоровья
		{
			life = false; // смерть
			game_over = true; // завершение игры
		}
	}
	void update(double time, RenderWindow& window) // метод обновления персонажа
	{
		if (x + speed_w * time > 0 && x + speed_w * time < map_w[level] * rect - w) // если не выйдем за границы карты по горизонтали, то
			x += speed_w * time; // двигаемся по горизонтали
		if (y + speed_h * time > 0 && y + speed_h * time < map_h[level] * rect - h) // если не выйдем за границы карты по вертикали, то
			y += speed_h * time; // двигаемся по вертикали
		w = frames[int(curr)][2]; // обновление размеров в соответствии с текущим кадром
		h = frames[int(curr)][3];
		speed_w = 0; // зануление скорости
		//conflict = conflict_right() && conflict_left();
		if (conflict_up()) // если столкновение сверху
			speed_h = 10 * acs * time; // толкаемся вниз
		if (!conflict_down()) // если снизу нет земли
		{
			jump = false; // отключение возможности прыжка
			speed_h += acs * time; // и включение ускорения свободного падения
		}
		else // если же стоим на земле, то
		{
			jump = true; // прыгать можно
			speed_h = 0; // скорости по вертикали нет
		}
		restart_game(start_pos[level][0], start_pos[level][1], window); // рестарт, если надо
		heal(); // сбор сердечек и монеток
		death(); // проверка смерти
		sprite.setPosition(x, y); // визуальное перемещение персонажа
		player_rect[0] = x; // запоминание координат и размеров (для корректной работы методов обработки столкновений класса Enemy)
		player_rect[1] = y;
		player_rect[2] = w;
		player_rect[3] = h;
	}
	void move(double time, RenderWindow& window) // метод движения
	{
		if (life) // если жив
		{
			if (conflict_down()) // если стоит на земле
			{
				curr += fps * time; // переключение кадров работает
				if (curr > 8)
					curr -= 8;
			}
			if ((Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) && !conflict_left()) // управление с помощью клавиш и визуальная смена кадров
			{
				speed_w = -hero_speed; dir = -1;
				sprite.setTextureRect(IntRect(frames[int(curr)][0] + frames[int(curr)][2], frames[int(curr)][1], -frames[int(curr)][2], frames[int(curr)][3]));
			}
			else if ((Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) && !conflict_right())
			{
				speed_w = hero_speed; dir = 1;
				sprite.setTextureRect(IntRect(frames[int(curr)][0], frames[int(curr)][1], frames[int(curr)][2], frames[int(curr)][3]));
			}
			if ((Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) && jump == true)
				speed_h = -1.6 * hero_speed;
		}
	}
	void view_map(Text& text1, Text& text2, Text& text3) // метод для привязки камеры к игроку
	{
		if (show_menu) // если мы в меню, то
		{
			view.setCenter(width / 2, height / 2); // камеру на исходное положение
			return;
		}
		double X = x, Y = y; // далее поиск центра камеры, в том числе если рядом край карты (то, что за ним не должно быть видно)
		if (X < width / 2)
			X = width / 2;
		else if (X > map_w[level] * rect - width / 2)
			X = map_w[level] * rect - width / 2;
		if (Y < height / 2)
			Y = height / 2;
		else if (Y > map_h[level] * rect - height / 2)
			Y = map_h[level] * rect - height / 2;
		view.setCenter(X, Y); // установка центра камеры
		s.setPosition(X - width / 2, Y - height / 2); // установка позиций шкалы здоровья,
		ss.setPosition(X - width / 2 + 300, Y - height / 2); // камня, который надо бросать,
		over.setPosition(view.getCenter() - Vector2f(30, 60)); // таблички "game over",
		congratulations.setPosition(view.getCenter() - Vector2f(190, 140)); // поздравлений с пройденным уровнем,
		text.setPosition(X - width / 2 + 50, Y - height / 2 + 200);
		text1.setPosition(X - width / 2 + 340, Y - height / 2 - 10); // и текстов, нужных для этого
		text2.setPosition(congratulations.getPosition() + Vector2f(60, 72));
		text3.setPosition(X - width / 2 + 200, Y - height / 2 + 100);
	}
};

class Map // класс карты
{
public:
	String file; // файл с текстурами
	Image image, ifinish; // картинка текущего рисуемого блока и таблички финиша
	Texture texture, tfinish; // их текстуры
	Sprite sprite, finish, congratulations; // и спрайты
	bool draw_finish; // нужно ли рисовать финиш
	Map(String F) // конструктор
	{
		draw_finish = false;
		file = F;
		image.loadFromFile("images/" + file);
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		ifinish.loadFromFile("images/finish.png");
		ifinish.createMaskFromColor(Color::White);
		tfinish.loadFromImage(ifinish);
		finish.setTexture(tfinish);
		finish.setTextureRect(IntRect(20, 20, 90, 80));
		congratulations.setTexture(tfinish);
		congratulations.setTextureRect(IntRect(209, 76, 637 - 209, 359 - 76));
	}
	void draw(RenderWindow& window) // рисование карты
	{
		for (int i = 0; i < map_h[level]; i++) // пробегаемся по массиву символов текущего уровня
			for (int j = 0; j < map_w[level]; j++)
			{
				if (m_map[level][i][j] == ' ') // рисование воздуха
					sprite.setTextureRect(IntRect(280, 30, rect, rect));
				else if (m_map[level][i][j] == '1') // рисование земли (1 - 9)
					sprite.setTextureRect(IntRect(43, 24, rect, rect));
				else if (m_map[level][i][j] == '2')
					sprite.setTextureRect(IntRect(193, 24, rect, rect));
				else if (m_map[level][i][j] == '3')
					sprite.setTextureRect(IntRect(368, 24, rect, rect));
				else if (m_map[level][i][j] == '4')
					sprite.setTextureRect(IntRect(43, 127, rect, rect));
				else if (m_map[level][i][j] == '5')
					sprite.setTextureRect(IntRect(200, 127, rect, rect));
				else if (m_map[level][i][j] == '6')
					sprite.setTextureRect(IntRect(368, 127, rect, rect));
				else if (m_map[level][i][j] == '7')
					sprite.setTextureRect(IntRect(43, 255, rect, rect));
				else if (m_map[level][i][j] == '8')
					sprite.setTextureRect(IntRect(217, 255, rect, rect));
				else if (m_map[level][i][j] == '9')
					sprite.setTextureRect(IntRect(368, 256, rect, rect));
				else if (m_map[level][i][j] == 'h') // рисование сердечка
					sprite.setTextureRect(IntRect(130, 240, rect, rect));
				else if (m_map[level][i][j] == 'm') // рисование монетки
					sprite.setTextureRect(IntRect(130, 50, rect, rect));
				sprite.setPosition(j * rect, i * rect);
				window.draw(sprite);
				if (m_map[level][i][j] == 'f') // рисование финиша
				{
					finish.setPosition(j * rect, i * rect);
					draw_finish = true;
				}
			}
		if (draw_finish)
		{
			window.draw(finish);
			draw_finish = false;
		}
	}
};

class Stone // класс камня
{
public:
	Image image; Texture texture; Sprite sprite; // картинка, текстура и спрайт
	int start_x, speed; // стартовая позиция и скорость
	bool used, fly; // хочет ли игрок использовать, в полёте ли камень
	Stone(Player& player) // конструктор
	{
		image.loadFromFile("images/stone.jpg");
		image.createMaskFromColor(Color::White);
		image.createMaskFromColor(Color(221, 221, 221));
		texture.loadFromImage(image);
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect(5, 8, 36, 33));
		start_x = player.x;
		speed = 0;
		sprite.setPosition(start_x, player.y);
		used = false;
		fly = false;
	}
	bool conflict_right() // методы конфликтов разъясняются в классе Enemy
	{
		int x = sprite.getPosition().x, y = sprite.getPosition().y, n = enemy_count[level], w = 36, h = 33;
		char c;
		for (int i = 1; i < h; ++i)
		{
			c = m_map[level][int(y + i) / rect][int(x + w) / rect];
			if (c >= 49 && c <= 57)
				return true;
		}
		for (int i = 0; i < n; ++i)
		{
			if (enemy[i]->life == true && x + w > enemy[i]->x && x + w < enemy[i]->x + enemy[i]->w / 2 && (y > enemy[i]->y && y < enemy[i]->y + enemy[i]->h || y + h > enemy[i]->y && y + h < enemy[i]->y + enemy[i]->h))
			{
				enemy[i]->health--; // при попадании камня во врага тот теряет здоровье
				return true;
			}
		}
		return  false;
	}
	bool conflict_left() // методы конфликтов разъясняются в классе Enemy
	{
		int x = sprite.getPosition().x, y = sprite.getPosition().y, n = enemy_count[level], w = 36, h = 33;
		char c;
		for (int i = 1; i < h; ++i)
		{
			c = m_map[level][int(y + i) / rect][int(x - 1) / rect];
			if (c >= 49 && c <= 57)
				return true;
		}
		for (int i = 0; i < n; ++i)
		{
			if (enemy[i]->life == true && x < enemy[i]->x + enemy[i]->w && x > enemy[i]->x + enemy[i]->w / 2 && (y > enemy[i]->y&& y < enemy[i]->y + enemy[i]->h || y + h > enemy[i]->y&& y + h < enemy[i]->y + enemy[i]->h))
			{
				enemy[i]->health--; // при попадании камня во врага тот теряет здоровье
				return true;
			}
		}
		return false;
	}
	void use(double time, Player& player, RenderWindow& window) // метод для использования камня
	{
		if (Keyboard::isKeyPressed(Keyboard::Space) && fly == false) // если нажат пробел
			used = true; // запоминаем, что игрок хочет использовать камень
		if (!Keyboard::isKeyPressed(Keyboard::Space) && used == true) // если пробел не нажат, а камень надо выпустить, то 
		{
			if (stones > 0) // если есть камни
			{
				start_x = player.x; // стартовая позиция равна позиции игрока
				used = false; // камень не надо использовать до нажатия пробела
				fly = true; // камень в полёте
				speed = hero_speed * 3 * player.dir; // задание скорости и направления
				stones--; // уменьшаем количество камней (можно получить за убийство врага, смотреть метод Enemy::death())
			}
			else // если камни закончились
				no_stones = true; // запоминаем это, потом выведем соответствующее сообщение
		}
		if (fly == true) // если камень в полёте
		{
			if (conflict_left() || conflict_right()) // если столкновение, то
				fly = false; // прекращение полёта
			if (abs(sprite.getPosition().x - start_x) < stone_fly && sprite.getPosition().x > 5 && sprite.getPosition().x < map_w[level] * rect - 5) // если НЕ преодолена максимальная дистанция
			{
				sprite.move(speed * time, 0); // летим дальше
				window.draw(sprite); // прорисовываем камень
			}
			else // иначе
			{
				fly = false; // прекращаем полёт
				speed = 0; // убираем скорость
				sprite.setPosition(player.x, player.y); // возвращаем на стартовую позицию
			}
		}
		else // если не в полёте
			sprite.setPosition(player.x, player.y); // двигаем вслед за игроком
	}
};

class Menu // класс меню (самый длинный и плохо написанный, но хорошо работающий)
{
public:
	Image image; Texture texture; std::vector<Sprite> sprite; String name; View view; // картинка, текстура и спрайт для меню, имя меню и камера для меню
	Sprite buy[4]; // кнопки покупок (для магазина)
	int curr, buttons, add; // текущая кнопка, количество кнопок, количество элементов "не кнопок"
	double x, y; // координаты верхнего левого угла меню
	bool plus, minus, in_this, enter, level_changed; // можно ли переместиться на кнопку ниже/выше, в этом ли мы меню, можно ли войти в меню, поменялся ли уровень
	Menu(RenderWindow& window, String str) // конструктор
	{
		view.reset(FloatRect(0, 0, width, height));
		view.setCenter(width / 2, height / 2);
		name = str; curr = 0; plus = false; minus = false; enter = false; level_changed = false;
		image.loadFromFile("images/menu.png");
		image.createMaskFromColor(Color::White);
		texture.loadFromImage(image);
		if (name == "main") // если это главное меню, проводим размещение объектов главного меню
		{
			buttons = 5; add = 1; in_this = true;
			x = width / 2 - main_menu[0][2] / 2; y = height / 2 - (main_menu[0][3] * buttons + 20 * (buttons - 1)) / 2;
			sprite.resize(buttons + add);
			for (int i = 0; i < buttons + add; ++i)
			{
				sprite[i].setTexture(texture);
				sprite[i].setTextureRect(IntRect(main_menu[i][0], main_menu[i][1], main_menu[i][2], main_menu[i][3]));
				if (i < buttons + add - 1)
					sprite[i].setPosition(x, y + (main_menu[i][3] + 20) * i);
			}
		}
		else if (name == "shop") // если это магазин, проводим размещение объектов магазина
		{
			buttons = 5; add = 4; in_this = false;
			sprite.resize(buttons + add);
			for (int i = 0; i < buttons + add; ++i)
			{
				sprite[i].setTexture(texture);
				sprite[i].setTextureRect(IntRect(shop[i][0], shop[i][1], shop[i][2], shop[i][3]));
				if (i < buttons + add - 2)
					sprite[i].setPosition(shop_pos[i][0], shop_pos[i][1]);
			}
			for (int i = 0; i < 4; ++i)
			{
				buy[i].setTexture(texture);
				buy[i].setTextureRect(IntRect(change[11][0], change[11][1], change[11][2], change[11][3]));
				buy[i].setPosition(shop_pos[i][0] + shop[i][2] + 10, shop_pos[i][1]);
			}
		}
		else if (name == "about") // если это меню "об игре", проводим размещение объектов меню "об игре"
		{
			buttons = 1; add = 2; in_this = false;
			sprite.resize(buttons + add);
			for (int i = 0; i < buttons + add; ++i)
			{
				sprite[i].setTexture(texture);
				sprite[i].setTextureRect(IntRect(about_game[i][0], about_game[i][1], about_game[i][2], about_game[i][3]));
				sprite[i].setPosition(about_game_pos[i][0], about_game_pos[i][1]);
			}
		}
		else if (name == "level") // если это меню выбора уровня, проводим размещение объектов меню выбора уровня
		{
			buttons = 11; add = 2; in_this = false;
			sprite.resize(buttons + add);
			for (int i = 0; i < buttons + add; ++i)
			{
				sprite[i].setTexture(texture);
				sprite[i].setTextureRect(IntRect(change[i][0], change[i][1], change[i][2], change[i][3]));
				if (i < buttons)
					sprite[i].setPosition(change_pos[i][0], change_pos[i][1]);
			}
		}
	}
	void update_menu(RenderWindow& window, Menu& m_shopping, Menu& m_about, Menu& m_lvl) // обновление ГЛАВНОГО МЕНЮ
	{
		if ((Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) && curr < buttons - 1) // переключение кнопок стрелками вверх/вниз
			plus = true;
		else if ((Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) && curr > 0)
			minus = true;
		if (!Keyboard::isKeyPressed(Keyboard::S) && !Keyboard::isKeyPressed(Keyboard::Down) && !Keyboard::isKeyPressed(Keyboard::W) && !Keyboard::isKeyPressed(Keyboard::Up))
			if (plus)
			{
				curr++;
				plus = false;
			}
			else if (minus)
			{
				curr--;
				minus = false;
			}
		if (curr != -1) // отрисовка кнопок
			sprite[sprite.size() - 1].setPosition(sprite[curr].getPosition() - Vector2f(3, 3));
		window.draw(sprite[sprite.size() - 1]);
		for (int i = 0; i < sprite.size() - 1; ++i)
			window.draw(sprite[i]);
		if (Keyboard::isKeyPressed(Keyboard::Enter)) // если нажат Enter
			enter = true; // просим войти по кнопке туда, куда хотим
		if (!Keyboard::isKeyPressed(Keyboard::Enter) && enter) // если Enter не нажат, но есть запрос на вход
		{
			enter = false; // удаляем запрос
			in_this = false; // уходим из этого меню
			if (curr == 0) // если начата новая игра
			{
				show_menu = false; // убираем меню
				new_game = true;
			}
			else if (curr == 1) // аналогичные переходы в другие меню
			{
				m_lvl.in_this = true;
				m_lvl.curr = level;
				m_shopping.in_this = false;
				m_about.in_this = false;
			}
			else if (curr == 2)
			{
				m_shopping.in_this = true;
				m_shopping.curr = 0;
				m_lvl.in_this = false;
				m_about.in_this = false;
			}
			else if (curr == 3)
			{
				m_about.in_this = true;
				m_about.curr = 0;
				m_shopping.in_this = false;
				m_lvl.in_this = false;
			}
			else if (curr == 4) // выход из игры с записью в файл данных о количестве монет, максимальном здоровье и покупках
			{
				show_menu = false;
				std::ofstream fout("images/progress.txt");
				fout << gold << std::endl << Health << std::endl << buyed[0] << ' ' << buyed[1] << ' ' << buyed[2] << ' ' << buyed[3] << std::endl;
				window.close();
			}
		}
	}
	void update_shopping(RenderWindow& window, Menu& menu, Player& player) // обновление МАГАЗИНА
	{
		Font font; // текст для магазина
		font.loadFromFile("images/CyrilicOld.TTF");
		Text text1(int_to_str(Health) + " / 7", font, 30), text2("Текущая стойкость:", font, 30), text3("В наличии:", font, 30), text4(int_to_str(gold), font, 30);
		text1.setFillColor(Color::Red); text2.setFillColor(Color::Red); text3.setFillColor(Color::Red); text4.setFillColor(Color::Red);
		text1.setPosition(44, 80); text2.setPosition(44, 40); text3.setPosition(width - 335, 40); text4.setPosition(width - 190, 40);
		if ((Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) && curr < buttons - 1) // переключение кнопок и их отрисовка
			plus = true;
		else if ((Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) && curr > 0)
			minus = true;
		if (!Keyboard::isKeyPressed(Keyboard::S) && !Keyboard::isKeyPressed(Keyboard::Down) && !Keyboard::isKeyPressed(Keyboard::W) && !Keyboard::isKeyPressed(Keyboard::Up))
			if (plus)
			{
				curr++;
				plus = false;
			}
			else if (minus)
			{
				curr--;
				minus = false;
			}
		if (curr != 4)
		{
			sprite[sprite.size() - 2].setPosition(sprite[curr].getPosition() - Vector2f(3, 3));
			window.draw(sprite[sprite.size() - 2]);
			for (int i = 0; i < sprite.size() - 2; ++i)
				window.draw(sprite[i]);
		}
		else
		{
			sprite[sprite.size() - 1].setPosition(sprite[curr].getPosition() - Vector2f(3, 3));
			window.draw(sprite[sprite.size() - 1]);
			for (int i = 0; i < sprite.size() - 2; ++i)
				window.draw(sprite[i]);
		}
		for (int i = 0; i < 4; ++i)
			if (buyed[i])
				window.draw(buy[i]);
		if (Keyboard::isKeyPressed(Keyboard::Enter)) // аналогичный вход
			enter = true;
		if (!Keyboard::isKeyPressed(Keyboard::Enter) && enter)
		{
			enter = false;
			if (curr == 0 && gold >= price[4] && !buyed[curr]) // покупки
			{
				Health++;
				gold -= price[4];
				buyed[curr] = true;
				player.health++;
			}
			else if (curr == 1 && gold >= price[5] && !buyed[curr])
			{
				Health++;
				gold -= price[5];
				buyed[curr] = true;
				player.health++;
			}
			else if (curr == 2 && gold >= price[6] && !buyed[curr])
			{
				Health++;
				gold -= price[6];
				buyed[curr] = true;
				player.health++;
			}
			else if (curr == 3 && gold >= price[7] && !buyed[curr])
			{
				Health++;
				gold -= price[7];
				buyed[curr] = true;
				player.health++;
			}
			else if (curr == 4) // выход в главное меню
			{
				in_this = false;
				menu.in_this = true;
			}
		}
		window.draw(text1); // пишем нужные тексты
		window.draw(text2);
		window.draw(text3);
		window.draw(text4);
	}
	void update_about(RenderWindow& window, Menu& menu) // обновление меню "ОБ ИГРЕ"
	{
		window.draw(sprite[2]); // рисование объектов
		window.draw(sprite[0]);
		window.draw(sprite[1]);
		if (Keyboard::isKeyPressed(Keyboard::Enter)) // выход в главное меню
			enter = true;
		if (!Keyboard::isKeyPressed(Keyboard::Enter) && enter)
		{
			enter = false;
			in_this = false;
			menu.in_this = true;
		}

	}
	void update_level(RenderWindow& window, Menu& menu, Player& player) // обновление МЕНЮ ВЫБОРА УРОВНЯ
	{
		if ((Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down)) && curr < buttons - 1) // переключение кнопок
			plus = true;
		else if ((Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up)) && curr > 0)
			minus = true;
		if (!Keyboard::isKeyPressed(Keyboard::S) && !Keyboard::isKeyPressed(Keyboard::Down) && !Keyboard::isKeyPressed(Keyboard::W) && !Keyboard::isKeyPressed(Keyboard::Up))
			if (plus)
			{
				curr++;
				plus = false;
			}
			else if (minus)
			{
				curr--;
				minus = false;
			}
		sprite[sprite.size() - 1].setPosition(sprite[curr].getPosition() - Vector2f(3, 3)); // отрисовка кнопок
		window.draw(sprite[sprite.size() - 1]);
		if (!level_changed)
			sprite[sprite.size() - 2].setPosition(sprite[0].getPosition() + Vector2f(266, 0));
		for (int i = 0; i < sprite.size() - 1; ++i)
			window.draw(sprite[i]);
		if (Keyboard::isKeyPressed(Keyboard::Enter)) // нажатие на кнопки
			enter = true;
		if (!Keyboard::isKeyPressed(Keyboard::Enter) && enter)
		{
			enter = false;
			if (curr != 10)
			{
				level = curr; // запоминаем выбранный уровень
				sprite[sprite.size() - 2].setPosition(sprite[curr].getPosition() + Vector2f(266, 0));
				level_changed = true;
				window.draw(sprite[sprite.size() - 2]);
				enemy.resize(enemy_count[level]); // изменяем размеры массива врагов на нужные
				player.x = start_pos[level][0], player.y = start_pos[level][1];
				if (level == 0) // задаём параметры врагов в зависимости от уровня
				{
					enemy[0] = new Enemy(3, 600, 840, 680);
					enemy[1] = new Enemy(3, 440, 640, 360);
					enemy[2] = new Enemy(3, 920, 1160, 1080);
					enemy[3] = new Enemy(3, 1520, 1760, 1080);
				}
				else if (level == 1)
				{
					enemy[0] = new Enemy(3, 47 * rect, 52 * rect, (103 - 79) * rect - skelet[0][3]);
					enemy[1] = new Enemy(3, 37 * rect, 47 * rect, (97 - 79) * rect - skelet[0][3]);
					enemy[2] = new Enemy(3, 11 * rect, 20 * rect, (94 - 79) * rect - skelet[0][3]);
					enemy[3] = new Enemy(3, 16 * rect, 23 * rect, (88 - 79) * rect - skelet[0][3]);
					enemy[4] = new Enemy(3, 39 * rect, 47 * rect, (85 - 79) * rect - skelet[0][3]);
				}
				//else if (level == 2)
				//{
				//	enemy[0] = new Enemy(3, 30 * rect, 40 * rect, 19 * rect - skelet[0][3]);
				//}
				// ПРИ СОЗДАНИИ НОВЫХ УРОВНЕЙ ЗДЕСЬ НАДО НАПИСАТЬ СООТВЕТСВУЮЩИЕ УСЛОВИЯ (ещё надо поменять соответствующие уровню массив строк m_map[level] и размер уровня в начале программы)
			}
			else // выход в главное меню
			{
				in_this = false;
				menu.in_this = true;
			}
		}
	}
};

int main() // ГЛАВНАЯ ФУНКЦИЯ ПРОГРАММЫ
{
	RenderWindow window(VideoMode(width, height), "Golden journey!"); // создаём окно
	window.setMouseCursorVisible(false); // скрываем курсор мыши
	Image icon; // создаём иконку окна
	icon.loadFromFile("images/icon.png");
	window.setIcon(32, 32, icon.getPixelsPtr());

	Clock clock;
	Map map("map.jpg"); // создаём карту
	std::ifstream fin("images/progress.txt"); // считываем из файла данные о монетах, максимальном здоровье и покупках
	fin >> gold >> Health >> buyed[0] >> buyed[1] >> buyed[2] >> buyed[3];
	fin.close();
	std::vector<Sprite*> s; // создаём массив сердечек для шкалы здоровья

	Font font; // загружаем шрифт для дальнейших текстов
	font.loadFromFile("images/CyrilicOld.TTF");

	Player player(start_pos[level][0], start_pos[level][1]); // создаём игрока
	enemy[0] = new Enemy(3, 600, 840, 680); // создаём врагов для уровня 1 (для корректной работы программы)
	enemy[1] = new Enemy(3, 440, 640, 360);
	enemy[2] = new Enemy(3, 920, 1160, 1080);
	enemy[3] = new Enemy(3, 1520, 1760, 1080);
	
	Stone stone(player); // даём игроку камень

	Menu menu(window, "main"); // создаём 4 меню
	Menu shopping(window, "shop");
	Menu about(window, "about");
	Menu lvl(window, "level");

	Text text1(int_to_str(stones) + "шт.", font, 40); // и нужные тексты
	text1.setFillColor(Color(100, 100, 100));

	Text text2("     Поздравляем!!!\nУровень " + int_to_str(level + 1) + " пройден!\n" + int_to_str(stones) + "      = " + int_to_str(stones * 2), font, 40);
	text2.setFillColor(Color::Blue);
	text2.setPosition(player.congratulations.getPosition() + Vector2f(60, 72));

	Text text3("Больше нет камней!", font, 40);
	text3.setFillColor(Color::Red);

	while (window.isOpen()) // и наконец ГЛАВНЫЙ ЦИКЛ ИГРЫ "ПОКА ОТКРЫТО ОКНО"
	{
		double time = clock.getElapsedTime().asMicroseconds(); // узнаём прошедшее время в микросекундах
		clock.restart(); // перезагружаем время
		time = time / game_speed; // для удобной скорости игры

		Event event; // обработка закрытия окна с запоминаем данных в файле
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				std::ofstream fout("images/progress.txt");
				fout << gold << std::endl << Health << std::endl << buyed[0] << ' ' << buyed[1] << ' ' << buyed[2] << ' ' << buyed[3] << std::endl;
				window.close();
			}
		}

		window.clear(Color(219, 221, 202)); // заливка окна нужным цветом 

		if (show_menu) // если мы в меню
		{
			window.setView(menu.view); // включаем камеру для меню и обновляем нужное меню
			if (to_menu_from_restart_menu == true)
			{
				to_menu_from_restart_menu = false;
				menu.in_this = true;
				menu.curr = 1;
			}
			if (menu.in_this)
				menu.update_menu(window, shopping, about, lvl);
			else if (shopping.in_this)
				shopping.update_shopping(window, menu, player);
			else if (about.in_this)
				about.update_about(window, menu);
			else if (lvl.in_this)
				lvl.update_level(window, menu, player);
		}
		else // если мы НЕ в меню
		{
			if (new_game == true) // если надо перезапустить уровень
			{
				new_game = false; // начинаем
				awarded = false; // не даём награду
				stones = 3; // обновляем количество камней
				no_stones = false;
				player.reset(start_pos[level][0], start_pos[level][1]); // переинициализируем игрока
				for (int i = 0; i < enemy_count[level]; ++i) // снимаем врагов с паузы и оживляем их
				{
					enemy[i]->stop = false;
					enemy[i]->life = true;
					enemy[i]->health = enemy[i]->was_health;
					enemy[i]->gave_stones = false;
				}
			}
			
			window.setView(player.view); // включаем камеру игрока
			player.update(time, window); // обновляем игрока

			if (pause == false)
			{
				map.draw(window); // рисуем карту
				for (int i = 0; i < enemy_count[level]; ++i) // рисуем врагов
				{
					enemy[i]->update(time);
					if (enemy[i]->life == true)
					{
						enemy[i]->move(time);
						window.draw(enemy[i]->sprite);
					}
				}

				player.move(time, window); // перемещаем игрока
				player.view_map(text1, text2, text3); // перемещаем камеру игрока
				window.draw(player.sprite); // рисуем игрока
				stone.use(time, player, window); // даём возможность использовать камень

				s.push_back(&(player.s)); // рисуем шкалу здоровья
				if (player.life == true || the_end == true)
					window.draw(*(s[0]));
				for (int i = 1; i < player.health; ++i)
				{
					s.push_back(&(player.s));
					s[i]->setPosition(s[i - 1]->getPosition() + Vector2f(rect, 0));
					window.draw(*(s[i]));
				}

				window.draw(player.ss); // выводим количество оставшихся камней
				text1.setString(int_to_str(stones) + "шт.");
				window.draw(text1);

				if (no_stones == true) // если нет камней
					window.draw(text3); // выводим соответствующее сообщение
			}

			if (game_over) // если конец игры
			{
				for (int i = 0; i < enemy_count[level]; ++i) // останавливаем врагов
					enemy[i]->stop = true;
				std::ofstream fout("images/progress.txt"); // пишем данные в файл
				fout << gold << std::endl << Health << std::endl << buyed[0] << ' ' << buyed[1] << ' ' << buyed[2] << ' ' << buyed[3] << std::endl;
				window.draw(player.over); // рисуем "game over"
				if (Keyboard::isKeyPressed(Keyboard::Enter)) // выходим в меню при нажатии Enter
					to_menu = true;
				if (!Keyboard::isKeyPressed(Keyboard::Enter) && to_menu == true)
				{
					to_menu = false;
					game_over = false;
					menu.curr = 1;
					show_menu = true;
					menu.in_this = true;
				}
			}

			if (the_end) // если уровень пройден
			{
				player.life = false;
				for (int i = 0; i < enemy_count[level]; ++i)
					enemy[i]->stop = true;
				window.draw(player.congratulations); // поздравляем игрока
				text2.setString("     Поздравляем!!!\nУровень " + int_to_str(level + 1) + " пройден!\n          " + int_to_str(stones) + "      = " + int_to_str(stones * 2));
				window.draw(text2);
				Texture tmp;
				tmp.loadFromFile("images/map.jpg");
				Sprite _stone_ = player.ss, _money_; // рисование награды
				_stone_.setPosition(player.view.getCenter() - Vector2f(15, -20));
				_money_.setTexture(tmp);
				_money_.setTextureRect(IntRect(130, 50, rect, rect));
				_money_.setPosition(player.view.getCenter() + Vector2f(100, 20));
				window.draw(_stone_);
				window.draw(_money_);
				std::ofstream fout("images/progress.txt"); // пишем данные в файл
				if (awarded == false) // награждаем
				{
					gold += stones * 2;
					awarded = true;
				}
				fout << gold << std::endl << Health << std::endl << buyed[0] << ' ' << buyed[1] << ' ' << buyed[2] << ' ' << buyed[3] << std::endl;
				if (Keyboard::isKeyPressed(Keyboard::Enter)) // выходим в меню при нажатии Enter
					to_menu = true;
				if (!Keyboard::isKeyPressed(Keyboard::Enter) && to_menu == true)
				{
					to_menu = false;
					the_end = false;
					menu.curr = 1;
					show_menu = true;
					menu.in_this = true;
				}
			}

			std::ofstream fout("images/progress.txt"); // пишем данные в файл
			fout << gold << std::endl << Health << std::endl << buyed[0] << ' ' << buyed[1] << ' ' << buyed[2] << ' ' << buyed[3] << std::endl;
		}
		window.display(); // обязательная функция
	}

	return 0; // конец :)
}
