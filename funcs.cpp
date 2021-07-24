#include "funcs.hpp"

float func_init (float x, float &y, float y_0, float R, bool rot)
{
    rot ? (y = sqrt(pow(R, 2) - pow(x, 2)) + y_0) : (y = -sqrt(pow(R, 2) - pow(x, 2)) + y_0);
    return y;
}

float define_velocity_x (float delta_x, float delta_y, float v)
{
    return v * cos(atan(delta_y / delta_x));
}

void init_cond (coord &o_pos, coord &f_pos, const float R, const float alpha, const int sign, const int x_direction)
{
    //получение координат центра
    o_pos.x = 0;
    o_pos.y = sign * x_direction * R * sin(M_PI/2 - alpha);
    //получение координат для функции
    f_pos.x = sign * x_direction * R * cos(M_PI/2 - alpha);
    f_pos.y = 0;
}

void field_mov (coord &f_pos, const coord o_pos, const float velocity, float &velocity_x, float &velocity_y, const float R, const bool rot_direction, const int sign)
{
    //запоминаем позицию во время предыдущей итерации
    float old_y = f_pos.y;
    //задаём псевдосмещение в 1 пиксель по х
    f_pos.x += 1.f * sign;
    velocity_x = 1.f * sign;
    //по псевдосмещению по х находим смещение по у
    if ((f_pos.x < R) && (f_pos.x > -R))
        velocity_y = func_init(f_pos.x, f_pos.y, o_pos.y, R, rot_direction) - old_y;
    //возвращаем всё назад
    f_pos.x -= 1.f * sign;
    f_pos.y = old_y;
    //находим реальное смещение по х
    velocity_x = define_velocity_x(velocity_x, velocity_y, velocity);
    //смещаем по х вправо
    f_pos.x += velocity_x * sign;
    //находим реальное смещение по y, смещаем по у
    if ((f_pos.x < R) && (f_pos.x > -R))
        velocity_y = func_init(f_pos.x, f_pos.y, o_pos.y, R, rot_direction) - old_y;
}

bool mouse_on_button (sf::Mouse m, sf::RenderWindow &w, button b)
{
    if ((m.getPosition(w).x > b.body.getPosition().x) && (m.getPosition(w).x < b.body.getPosition().x + b.body.getLocalBounds().width))
    {
        if ((m.getPosition(w).y > b.body.getPosition().y) && (m.getPosition(w).y < b.body.getPosition().y + b.body.getLocalBounds().height))
            return  true;
        else
            return false;
    }
    else
        return false;
}

void init_button (button &b, float pos_x, float pos_y, sf::Color b_color, string text, sf::Font &font, sf::Color t_color, float t_size)
{
    //инициализация текста
    b.text.setFont(font);
    b.text.setString(text);
    b.text.setCharacterSize(t_size);
    b.text.setFillColor(t_color);
    b.text.setPosition(pos_x + b.text.getLocalBounds().height / 2.f, pos_y);
    //инициализация тела
    sf::RectangleShape sh_b(sf::Vector2f(b.text.getLocalBounds().width * 1.045f + b.text.getLocalBounds().height, b.text.getLocalBounds().height * 2));
    b.body = sh_b;
    b.body.setFillColor(b_color);
    b.body.setPosition(pos_x, pos_y);
}

void color_button (button &b, sf::Color b_color, sf::Color t_color)
{
    b.body.setFillColor(b_color);
    b.text.setFillColor(t_color);
}

void init_textbox (textbox &tb, float pos_x, float pos_y, float size_x, float size_y, string text, sf::Font &font, string lab, sf::Font &font_lab, var_connect var, sf::RenderWindow &window)
{
    //инициализация текста
    tb.text.setFont(font);
    tb.text.setString(text);
    tb.text.setCharacterSize(size_y);
    tb.text.setFillColor(sf::Color(0, 0, 0, 128));
    tb.text.setPosition(pos_x + size_x * 0.01f, pos_y);
    //инициализация тела
    sf::RectangleShape sh_b(sf::Vector2f(size_x, tb.text.getLocalBounds().height * 2));
    tb.body = sh_b;
    tb.body.setFillColor(sf::Color(103, 128, 159, 255));
    tb.body.setPosition(pos_x, pos_y);
    //инициализация лейбла
    tb.label.setFont(font_lab);
    tb.label.setString(lab);
    tb.label.setCharacterSize(size_y * 0.8f);
    tb.label.setFillColor(sf::Color::White);
    tb.label.setPosition(pos_x, pos_y - size_y * 1.03);
    //установление связи с переменной
    tb.var = var;
    //установление вида
    sf::View tb_view(sf::FloatRect(tb.label.getPosition().x, tb.label.getPosition().y, tb.body.getLocalBounds().width, tb.body.getPosition().y + tb.body.getLocalBounds().height- tb.label.getPosition().y));
    tb_view.setViewport(sf::FloatRect((float)tb.label.getPosition().x/window.getSize().x, (float)(tb.label.getPosition().y/window.getSize().y),
                                      (float)tb.body.getLocalBounds().width/window.getSize().x, (float)((tb.body.getPosition().y + tb.body.getLocalBounds().height - tb.label.getPosition().y)/window.getSize().y)));
    tb.v = tb_view;
}

bool mouse_on_textbox (sf::Mouse m, sf::RenderWindow &w, textbox tx)
{
    if ((m.getPosition(w).x > tx.body.getPosition().x) && (m.getPosition(w).x < tx.body.getPosition().x + tx.body.getLocalBounds().width))
    {
        if ((m.getPosition(w).y > tx.body.getPosition().y) && (m.getPosition(w).y < tx.body.getPosition().y + tx.body.getLocalBounds().height))
            return  true;
        else
            return false;
    }
    else
        return false;
}

void mouse_on_textbox_all (sf::RenderWindow &window, sf::Mouse mouse, textbox *arr_tb, int arr_size, bool &flag)
{
    sf::Cursor cursor_text;
    cursor_text.loadFromSystem(sf::Cursor::Text);
    sf::Cursor cursor_arrow;
    cursor_arrow.loadFromSystem(sf::Cursor::Arrow);
    
    for (int i = 0; i < arr_size; i++)
    {
        if (mouse_on_textbox(mouse, window, arr_tb[i]))
            flag = true;
    }
    
    flag ? window.setMouseCursor(cursor_text) : window.setMouseCursor(cursor_arrow);
    flag = false;
}

void var_update (float &var, textbox &tb, sf::Event event)
{
    try
    {
        if ((char)event.text.unicode != '\b')
        {
            tb.text_s.push_back((char)event.text.unicode);
            tb.text.setString(tb.text_s);
            if (tb.text.getLocalBounds().width > tb.body.getLocalBounds().width - tb.body.getSize().x * 0.01f)
                tb.text.setPosition(tb.body.getPosition().x + tb.body.getLocalBounds().width - tb.text.getLocalBounds().width - tb.body.getSize().x * 0.01f, tb.text.getPosition().y);
        }
        else
        {
            if (tb.text_s.length() != 0)
            {
                char del = tb.text_s[tb.text_s.length() - 1];
                tb.text_s.pop_back();
                tb.text.setString(tb.text_s);
                if (tb.text.getLocalBounds().width > tb.body.getLocalBounds().width)
                    tb.text.setPosition(tb.body.getPosition().x + tb.body.getLocalBounds().width - tb.text.getLocalBounds().width - tb.body.getSize().x * 0.01f, tb.text.getPosition().y);
                else
                    tb.text.setPosition(tb.body.getPosition().x + tb.body.getSize().x * 0.01f, tb.text.getPosition().y);
            }
        }
        var = stof(tb.text_s);
    }
    catch (const exception &ex)
    {
        cout << "Вызвано исключение: "<< ex.what();
    }
}

void init_interface_text (sf::Text &text, sf::Font &font, sf::Color color, float size, float x_pos, float y_pos, string str)
{
    text.setFont(font);
    text.setFillColor(color);
    text.setCharacterSize(size);
    text.setPosition(x_pos, y_pos);
    text.setString(str);
}
