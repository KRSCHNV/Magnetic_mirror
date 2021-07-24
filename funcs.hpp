#ifndef funcs_hpp
#define funcs_hpp

#include <stdio.h>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
using namespace std;

struct coord //структура для хранения координат
{
    float x, y;
};

struct button //структура кнопки
{
    sf::RectangleShape body;
    sf::Text text;
};

enum var_connect {var_velocity, var_alpha, var_mas, var_q, var_B}; //перечисляемый тип для текстбокса (установление связи с переменной)

struct textbox //структура текстбокса
{
    sf::RectangleShape body;
    sf::Text text;
    sf::Text label;
    string text_s;
    string label_s;
    bool clicked = false;
    var_connect var;
    sf::View v;
};

float func_init (float x, float &y, float y_0, float R, bool rot); //инициализация функции для обеих половин окружности (верхней и нижней)
float define_velocity_x (float delta_x, float delta_y, float v); //определение реального смещения по оси x по псеводосмещениям и модулю скорости
void field_mov (coord &f_pos, const coord o_pos, const float velocity, float &velocity_x, float &velocity_y, const float R, const bool rot_direction, const int sign); //расчёт траектории частицы внутри поля
void init_cond (coord &o_pos, coord &f_pos, const float R, const float alpha, const int sign, const int x_direction); //расчёт начальных условий на границе
bool mouse_on_button (sf::Mouse m, sf::RenderWindow &w, button b); //расположена ли мышь на кнопке
void init_button (button &b, float pos_x, float pos_y, sf::Color b_color, string text, sf::Font &font, sf::Color t_color, float t_size); //инициализация кнопки
void color_button (button &b, sf::Color b_color, sf::Color t_color); //изменение стиля цвета
void init_textbox (textbox &tb, float pos_x, float pos_y, float size_x, float size_y, string text, sf::Font &font, string lab, sf::Font &font_lab, var_connect var, sf::RenderWindow &window); //инициализация текстбокса
bool mouse_on_textbox (sf::Mouse m, sf::RenderWindow &w, textbox tx); //расположена ли мышь на текстбоксе
void mouse_on_textbox_all (sf::RenderWindow &window, sf::Mouse mouse, textbox *arr_tb, int arr_size, bool &flag); //расположена ли мышь на каком-нибудь текстбоксе
void var_update (float &var, textbox &tb, sf::Event event); //обновление переменной (для текстбокса)
void init_interface_text (sf::Text &text, sf::Font &font, sf::Color color, float size, float x_pos, float y_pos, string str); //создать текст

#endif /* funcs_hpp */
