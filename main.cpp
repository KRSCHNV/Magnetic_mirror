#include "funcs.hpp"

int main(int, char const**)
{
    //ОСНОВНАЯ ЧАСТь
    //Окно
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Magnetic mirror", sf::Style::Close | sf::Style::Titlebar);
    sf::View Camera(sf::Vector2f(490.f, 360.f), sf::Vector2f(980.f, 720.f));
    Camera.setViewport(sf::FloatRect(0.f,0.f, 0.765625f, 1.f));
    int framerate = 60;
    window.setFramerateLimit(framerate);
    float zoomed = 1;
    //Частица
    float particle_radius = 10.f;
    sf::CircleShape particle(particle_radius);
    particle.setFillColor(sf::Color(0, 0, 0, 0));
    particle.setOrigin(particle_radius, particle_radius);
    //Граница
    sf::RectangleShape devide_line(sf::Vector2f(window.getSize().x, 2.f));
    devide_line.setPosition(0.f, 360.f);
    devide_line.setFillColor(sf::Color::Black);
    sf::View devide_line_view(sf::Vector2f(490.f, 360.f), sf::Vector2f(980.f, 2.f));
    devide_line_view.setViewport(sf::FloatRect(0.f, 0.5f, 1.f, 0.00278f));
    //Переменные для симуляции
    int x_direction;
    float velocity, alpha, velocity_x, velocity_y, mas, q, B, R;
    bool was_in_field, rot_direction, rot_direction2, was_on_border, button_pressed = false;
    coord o_pos, f_pos;
    
    //ИНТЕРФЕЙС
    //шрифты
    sf::Font QlassikBold;
    QlassikBold.loadFromFile("/Users/vladimirkorsunov/Desktop/sfmltest/sfmltest/QlassikBold_TB.otf");
    //кнопки
    button go_button, def_view_button, electron_button;
    init_button(go_button, 1000, 330, sf::Color::White, "Go!", QlassikBold, sf::Color::Red, 30);
    init_button(def_view_button, 1000, 390, sf::Color::White, "Default view", QlassikBold, sf::Color::Red, 30);
    init_button(electron_button, 1000, 450, sf::Color::White, "Electron", QlassikBold, sf::Color::Red, 30);
    button arr_but[] = {go_button, def_view_button, electron_button};
    int arr_but_size = sizeof(arr_but)/sizeof(arr_but[0]);
    //текстбоксы
    textbox velocity_tb, alpha_tb, mas_tb, q_tb, B_tb;
    init_textbox(velocity_tb, 1000, 30, 200, 25, "Enter text...", QlassikBold, "Velocity:", QlassikBold, var_velocity, window);
    init_textbox(alpha_tb, 1000, 90, 200, 25, "Enter text...", QlassikBold, "Alpha:", QlassikBold, var_alpha, window);
    init_textbox(mas_tb, 1000, 150, 200, 25, "Enter text...", QlassikBold, "Mass:", QlassikBold, var_mas, window);
    init_textbox(q_tb, 1000, 210, 200, 25, "Enter text...", QlassikBold, "Charge:", QlassikBold, var_q, window);
    init_textbox(B_tb, 1000, 270, 200, 25, "Enter text...", QlassikBold, "Magnetic induction:", QlassikBold, var_B, window);
    textbox arr_tb[] = {velocity_tb, alpha_tb, mas_tb, q_tb, B_tb};
    int arr_tb_size = sizeof(arr_tb)/sizeof(arr_tb[0]);
    bool flag_tb = false;
    //текст
    sf::Text warning, vel_text, rad_text, kg_text, c_text, t_text, zoom_info;
    init_interface_text(warning, QlassikBold, sf::Color(0, 0, 0, 0), 30.f, 1000.f, 510.f, "Enter data!");
    init_interface_text(vel_text, QlassikBold, sf::Color::White, 22.f, 1207.f, 30.f, "Px/60fr");
    init_interface_text(rad_text, QlassikBold, sf::Color::White, 22.f, 1207.f, 90.f, "Rad");
    init_interface_text(kg_text, QlassikBold, sf::Color::White, 22.f, 1207.f, 150.f, "Kg");
    init_interface_text(c_text, QlassikBold, sf::Color::White, 22.f, 1207.f, 210.f, "C");
    init_interface_text(t_text, QlassikBold, sf::Color::White, 22.f, 1207.f, 270.f, "T");
    init_interface_text(zoom_info, QlassikBold, sf::Color::White, 30.f, 1000.f, 670.f, "Zoom: 1.000000");
    sf::Text arr_tx[] = {vel_text, rad_text, kg_text, c_text, t_text, zoom_info};
    int arr_tx_size = sizeof(arr_tx)/sizeof(arr_tx[0]);
    //мышь
    sf::Mouse mouse;
    sf::Cursor cursor_text, cursor_arrow;
    cursor_text.loadFromSystem(sf::Cursor::Text);
    cursor_arrow.loadFromSystem(sf::Cursor::Arrow);
    //формы
    sf::View interface_view(sf::Vector2f(1130.f, 360.f), sf::Vector2f(300.f, 720.f));
    interface_view.setViewport(sf::FloatRect(0.765625f, 0.f, 0.234375f, 1.f));
    sf::RectangleShape background(sf::Vector2f(300.f, 720.f));
    background.setFillColor(sf::Color::Black);
    background.setOutlineThickness(-3.f);
    background.setOutlineColor(sf::Color(108, 122, 137, 255));
    background.setPosition(980.f, 0.f);
    
    // программа работает, пока окно открыто
        while (window.isOpen())
        {
            //проверяем, наведена ли мышь на кнопку запуска
            for (int i = 0; i < arr_but_size; i++)
                mouse_on_button(mouse, window, arr_but[i]) ? color_button(arr_but[i], sf::Color::Red, sf::Color::White) : color_button(arr_but[i], sf::Color::White, sf::Color::Red);
            //проверяем, наведена ли мышь на текстбокс
            mouse_on_textbox_all(window, mouse, arr_tb, arr_tb_size, flag_tb);
            
            //выполняем, если была нажата кнопка
            if (button_pressed)
            {
                if (was_on_border && particle.getPosition().y <= 0)
                    button_pressed = false;
                
                //движение частицы вне магнитного поля с постоянной скоростью
                if ((particle.getPosition().y < devide_line.getPosition().y) && (particle.getPosition().y > particle_radius))
                {
                    velocity_x = velocity * cos(alpha);
                    was_in_field ? velocity_y = -velocity * sin(alpha) : velocity_y = velocity * sin(alpha);
                    particle.move(velocity_x, velocity_y);
                }
                
                //расчитываем начальные условия на границе
                if ((particle.getPosition().y >= devide_line.getPosition().y) && !was_on_border)
                {
                    rot_direction ? init_cond(o_pos, f_pos, R, alpha, -1, x_direction) : init_cond(o_pos, f_pos, R, alpha, 1, x_direction);
                    was_on_border = true;
                }
                
                //перемещение частицы в поле с постоянной скоростью
                if (particle.getPosition().y >= devide_line.getPosition().y)
                {
                    //движение ТОЛЬКО по нижней части окружности
                    if(rot_direction)
                    {
                        //считаем смещение по осям x и y
                        field_mov (f_pos, o_pos, velocity, velocity_x, velocity_y, R, rot_direction, 1 * x_direction);
                        //смещаем реальный объект для отрисовки
                        particle.move(velocity_x * x_direction, velocity_y);
                    }
                    //смешанное движение
                    else
                    {
                        if (rot_direction2)
                        {
                            //считаем смещение по осям x и y
                            field_mov (f_pos, o_pos, velocity, velocity_x, velocity_y, R, rot_direction, 1 * x_direction);
                            //смещаем реальный объект для отрисовки
                            particle.move(velocity_x * x_direction, velocity_y);
                            //смена направления частицы по оси x (переход на движение по нижней части окружности)
                            if (f_pos.x * x_direction > R)
                                rot_direction2 = false;
                        }
                        else if (!rot_direction2)
                        {
                            //считаем смещение по осям x и y
                            field_mov (f_pos, o_pos, velocity, velocity_x, velocity_y, R, !rot_direction, -1 * x_direction);
                            //смещаем реальный объект для новой отрисовки
                            particle.move(-velocity_x * x_direction, velocity_y);
                            //смена направления частицы по оси x (переход на движение по верхней части окружности)
                            if (f_pos.x * x_direction < -R)
                                rot_direction2 = true;
                        
                        }
                    }
                    //для понимания того, что частица уже была в магнитном поле
                    was_in_field = true;
                }
            }
            window.clear(sf::Color(189, 195, 199, 255));
            //отрисовка гриницы
            window.setView(devide_line_view);
            window.draw(devide_line);
            //отрисовка основной части
            window.setView(Camera);
            window.draw(particle);
            //отрисовка интерфейса
            window.setView(interface_view);
            window.draw(background);
            window.draw(warning);
            for (int i = 0; i < arr_tx_size; i++)
                window.draw(arr_tx[i]);
            for (int i = 0; i < arr_but_size; i++)
            {
                window.draw(arr_but[i].body);
                window.draw(arr_but[i].text);
            }
            for (int i = 0; i < arr_tb_size; i++)
            {
                window.setView(arr_tb[i].v);
                window.draw(arr_tb[i].body);
                window.draw(arr_tb[i].text);
                window.draw(arr_tb[i].label);
            }
            window.display();
            
            // проверка событий окна
            sf::Event event;
            while (window.pollEvent(event))
            {
                //кнопка закрытия
                if (event.type == event.Closed)
                    window.close();
                //нажатие на кнопку мыши
                if (event.type == event.MouseButtonPressed)
                {
                    if (mouse_on_button(mouse, window, go_button))
                    {
                        bool initialisated = true;
                        for (int i = 0; i < arr_tb_size; i++)
                            if (arr_tb[i].text_s == "") {initialisated = false; break;}
                        
                        if (initialisated)
                        {
                            q > 0 ? particle.setFillColor(sf::Color::Red) : particle.setFillColor(sf::Color::Blue);
                            
                            button_pressed = true; //запуск визуализации
                            was_in_field = false; //частица еще не была в поле
                            rot_direction2 = true; //переменная для смены направления движения при смешанном движении
                            was_on_border = false; //частица еще не была на границе
                                
                            velocity_x = velocity * cos(alpha); //скорость по оси х
                            velocity_y = velocity * sin(alpha); //скорость по оси у
                            
                            //определение вида движения (прямое или смешанное)
                            if (((B > 0) && (velocity_x > 0) && (q < 0)) || ((B < 0) && (velocity_x > 0) && (q > 0)) || ((B < 0) && (velocity_x < 0) && (q < 0)) || ((B > 0) && (velocity_x < 0) && (q > 0)))
                                rot_direction = true;
                            else
                                rot_direction = false;
                                
                            R = (mas * velocity * framerate) / (abs(q) * abs(B)); //радиус траектории
                            particle.setPosition(490.f, particle_radius + 1);
                                
                            //определение знака движения частицы относительно оси х
                            velocity_x > 0 ? x_direction = 1 : x_direction = -1;
                            
                            warning.setFillColor(sf::Color(0, 0, 0, 0));
                        }
                        else
                        {
                            warning.setFillColor(sf::Color::Red);
                        }
                    }
                    else if (mouse_on_button(mouse, window, def_view_button))
                    {
                        Camera = window.getDefaultView();
                        arr_tx[5].setString("Zoom: 1.000000");
                    }
                    else if (mouse_on_button(mouse, window, electron_button))
                    {
                        for (int i = 0; i < arr_tb_size; i++)
                        {
                            switch (arr_tb[i].var) {
                                case var_velocity:
                                    arr_tb[i].text_s = "10";
                                    arr_tb[i].text.setPosition(arr_tb[i].body.getPosition().x + arr_tb[i].body.getLocalBounds().width * 0.01, arr_tb[i].text.getPosition().y);
                                    arr_tb[i].text.setString(arr_tb[i].text_s);
                                    velocity = stof(arr_tb[i].text_s);
                                    break;
                                case var_alpha:
                                    arr_tb[i].text_s = "1";
                                    arr_tb[i].text.setPosition(arr_tb[i].body.getPosition().x + arr_tb[i].body.getLocalBounds().width * 0.01, arr_tb[i].text.getPosition().y);
                                    arr_tb[i].text.setString(arr_tb[i].text_s);
                                    alpha = stof(arr_tb[i].text_s);
                                    break;;
                                case var_mas:
                                    arr_tb[i].text_s = "9.1e-31";
                                    arr_tb[i].text.setPosition(arr_tb[i].body.getPosition().x + arr_tb[i].body.getLocalBounds().width * 0.01, arr_tb[i].text.getPosition().y);
                                    arr_tb[i].text.setString(arr_tb[i].text_s);
                                    mas = stof(arr_tb[i].text_s);
                                    break;;
                                case var_q:
                                    arr_tb[i].text_s = "-1.6e-19";
                                    arr_tb[i].text.setPosition(arr_tb[i].body.getPosition().x + arr_tb[i].body.getLocalBounds().width * 0.01, arr_tb[i].text.getPosition().y);
                                    arr_tb[i].text.setString(arr_tb[i].text_s);
                                    q = stof(arr_tb[i].text_s);
                                    break;;
                                case var_B:
                                    arr_tb[i].text_s = "-1.7e-11";
                                    arr_tb[i].text.setPosition(arr_tb[i].body.getPosition().x + arr_tb[i].body.getLocalBounds().width * 0.01, arr_tb[i].text.getPosition().y);
                                    arr_tb[i].text.setString(arr_tb[i].text_s);
                                    B = stof(arr_tb[i].text_s);
                                    break;;
                                default:
                                    break;
                            }
                        }
                    }
                    //кликнут ли какой-нибудь текстбокс
                    for (int i = 0; i < arr_tb_size; i++)
                    {
                        if (mouse_on_textbox(mouse, window, arr_tb[i]))
                        {
                            if (!arr_tb[i].text_s.size())
                                arr_tb[i].text.setString("");

                            arr_tb[i].body.setFillColor(sf::Color::White);
                            arr_tb[i].text.setFillColor(sf::Color(0, 0, 0, 255));
                            arr_tb[i].clicked = true;
                        }
                        else
                        {
                            if (!arr_tb[i].text_s.size() && arr_tb[i].clicked)
                            {
                                arr_tb[i].text.setString("Enter text...");
                            }

                            arr_tb[i].body.setFillColor(sf::Color(103, 128, 159, 255));
                            arr_tb[i].text.setFillColor(sf::Color(0, 0, 0, 128));
                            arr_tb[i].clicked = false;
                        }
                    }
                }
                //нажатие на кнопку клавиатуры в текстбоксе
                if (event.type == event.TextEntered)
                {
                    for (int i = 0; i < arr_tb_size; i++)
                    {
                        if (arr_tb[i].clicked && (((event.text.unicode >= 48) && (event.text.unicode <= 57)) || event.text.unicode == 101 || event.text.unicode == 46 || event.text.unicode == 45 || (char)event.text.unicode == '\b'))
                        {
                            switch (arr_tb[i].var) {
                                case var_velocity:
                                    var_update(velocity, arr_tb[i], event);
                                    break;
                                case var_alpha:
                                    var_update(alpha, arr_tb[i], event);
                                    break;
                                case var_mas:
                                    var_update(mas, arr_tb[i], event);
                                    break;
                                case var_q:
                                    var_update(q, arr_tb[i], event);
                                    break;
                                case var_B:
                                    var_update(B, arr_tb[i], event);
                                    break;
                                    
                                default:
                                    break;
                            }
                        }
                    }
                }
                //скролинг колеса мыши
                if (event.type == event.MouseWheelScrolled)
                {
                    if (mouse.getPosition(window).x < background.getPosition().x)
                    {
                        if (event.mouseWheelScroll.delta > 0)
                        {
                            Camera.zoom(1.05f);
                            zoomed *= 1.05f;
                        }
                        else if (event.mouseWheelScroll.delta < 0)
                        {
                            Camera.zoom(0.95f);
                            zoomed *= 0.95f;
                        }
                        arr_tx[5].setString("Zoom: " + to_string(zoomed));
                    }
                }
            }
        }
    return 0;
}
