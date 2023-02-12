#include "game.h"

float vector2f_magnitude(Vector2f v1, Vector2f v2)
{
    return (sqrtf(powf(v2.x - v1.x, 2.0f) + powf(v2.y - v1.y, 2.0f)));
}

bool circle_circle_collision(CircleShape c1, CircleShape c2)
{
    float magnitude = vector2f_magnitude(c1.getPosition(), c2.getPosition());
    if (magnitude < c1.getRadius() + c2.getRadius())
        return (true);
    return (false);
}

void add_physics_obj(t_game &game, int index, Vector2f pos)
{
    t_physics_obj   physics_obj;

    physics_obj.circle.setPosition(pos);
    physics_obj.circle.setOrigin(Vector2f(50.0f, 50.0f));
    physics_obj.circle.setFillColor(Color::Green);
    physics_obj.circle.setRadius(50.0f);
    physics_obj.v = Vector2f(0.0f, 0.0f);
    physics_obj.valid = true;
    physics_obj.id = index;
    physics_obj.a = Vector2f(0.0f, 0.0f);
    physics_obj.mass = 100.0f;
    int i = 0;
    int found = 0;
    while (found < game.obj_count)
    {
        if (game.physics_obj[i].valid)
        {
            if (circle_circle_collision(game.physics_obj[i].circle, physics_obj.circle))
                return;
            found++;
        }
        i++;
    }
    game.physics_obj[index] = physics_obj;
    game.obj_count++;
}

void remove_physics_obj(t_game& game, int index)
{
    game.physics_obj[index].valid = false;
    game.obj_count--;
}

void handle_events(RenderWindow &window, t_game &game)
{
    Event   event;
    CircleShape *circle = &game.physics_obj[0].circle;

    while (window.pollEvent(event))
    {
        if (event.type == Event::Closed)
            window.close();
        if (Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();
        if (Mouse::isButtonPressed(Mouse::Left))
        {
            if (game.obj_count < game.obj_max)
            {
                int i = 0;
                while (i < game.obj_max)
                {
                    if (game.physics_obj[i].valid == false)
                        break;
                    i++;
                }
                add_physics_obj(game, i, window.mapPixelToCoords(Mouse::getPosition(window)));
            }
        }
        if (Mouse::isButtonPressed(Mouse::Right))
        {
            if (game.ballselected == false)
            {
                int i = 0;
                int found = 0;
                while (found < game.obj_count)
                {
                    if (game.physics_obj[i].valid)
                    {
                        if (game.physics_obj[i].circle.getGlobalBounds().contains(window.mapPixelToCoords(Mouse::getPosition(window))))
                        {
                            game.ballselected = true;
                            game.ballindex = i;
                            // game.physics_obj[i].circle.setPosition(window.mapPixelToCoords(Mouse::getPosition(window)) - Vector2f(50.0f, 50.0f));
                            break;
                        }
                        found++;
                    }
                    i++;
                }
            }
        }
        else if (game.ballselected && game.physics_obj[game.ballindex].valid)
        {
            game.ballselected = false;
            game.physics_obj[game.ballindex].v = 5.0f * (game.physics_obj[game.ballindex].circle.getPosition() - window.mapPixelToCoords(Mouse::getPosition(window)));
        }


    }   
}

void update_gravity(t_game& game)
{
    int found = 0;
    int i = 0;

    while (found < game.obj_count)
    {
        t_physics_obj* p = &game.physics_obj[i];
        if (p->valid)
        {
         //   p->v.y += 9.81f * game.elapsed.asSeconds();
            p->a = -p->v * 0.8f;
            p->v += p->a * game.elapsed.asSeconds();
            p->circle.move(p->v * game.elapsed.asSeconds());
            if (fabs(p->v.x * p->v.x + p->v.y * p->v.y) < 0.01f)
                p->v = Vector2f(0.0f, 0.0f);
            found++;
        }
        i++;
    }
}

bool checkcirclecollide(float x1, float y1, float r1, float x2, float y2, float r2) {
    return fabs((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)) < (r1 + r2) * (r1 + r2);
}

void check_collision(t_game &game, int index)
{
    int found = 1;
    int i = 0;
    float mass1 = 100.0f;
    float mass2 = 100.0f;

    t_physics_obj   *u1 = &game.physics_obj[index];
    Vector2f p1 = game.physics_obj[index].circle.getPosition();
    Vector2f v1 = game.physics_obj[index].v * game.elapsed.asSeconds();
    Vector2f a1 = game.physics_obj[index].a;
    while (found < game.obj_count)
    {
       // t_physics_obj* p = &game.physics_obj[i];
        if (game.physics_obj[i].valid && i != index)
        {
            t_physics_obj   *u2 = &game.physics_obj[i];
            Vector2f p2 = game.physics_obj[i].circle.getPosition();
            Vector2f v2 = game.physics_obj[i].v * game.elapsed.asSeconds();
            Vector2f a2 = game.physics_obj[i].a;

            if (checkcirclecollide(p1.x, p1.y, u1->circle.getRadius(), p2.x, p2.y, u2->circle.getRadius()))
            {
                float fDistance = vector2f_magnitude(p1, p2);

                float fOverlap = 0.5f * (fDistance - u1->circle.getRadius() - u2->circle.getRadius());

                Vector2f pos;
                pos.x = fOverlap * (u1->circle.getPosition().x - u2->circle.getPosition().x) / fDistance;
                pos.y = fOverlap * (u1->circle.getPosition().y - u2->circle.getPosition().y) / fDistance;
                u1->circle.setPosition(u1->circle.getPosition() - pos);
                u2->circle.setPosition(u2->circle.getPosition() + pos);

                Vector2f n = (u2->circle.getPosition() - u1->circle.getPosition()) / fDistance;

                Vector2f t = Vector2f(-n.x, n.y);

                float dptan1 = u1->v.x * t.x + u1->v.y * t.y;
                float dptan2 = u2->v.x * t.x + u2->v.y * t.y;

                float dpnorm1 = u1->v.x * n.x + u1->v.y * n.y;
                float dpnorm2 = u2->v.x * n.x + u2->v.y * n.y;

                float m1 = (dpnorm1 * (u1->mass - u2->mass) + 2.0f * u2->mass * dpnorm2) / (u1->mass + u2->mass);
                float m2 = (dpnorm2 * (u2->mass - u1->mass) + 2.0f * u1->mass * dpnorm1) / (u1->mass + u2->mass);

                u1->v.x = t.x * dptan1 + n.x * m1;
                u1->v.y = t.y * dptan1 + n.y * m1;
                u2->v.x = t.x * dptan2 + n.x * m2;
                u2->v.y = t.y * dptan2 + n.y * m2;
            }
            found++;
        }
        i++;
    }
}

void apply_collision(t_game &game)
{
    int found = 0;
    int i = 0;

    while (found < game.obj_count)
    {
        t_physics_obj* p = &game.physics_obj[i];
        if (p->valid)
        {
            check_collision(game, i);
            found++;
        }
        i++;
    }
}

void update_physics(RenderWindow& window, t_game& game)
{
    int found = 0;
    int i = 0;

    update_gravity(game);

    apply_collision(game);
    while (found < game.obj_count && i < game.obj_max)
    {
        t_physics_obj* p = &game.physics_obj[i];
        if (p->valid)
        {
           // p->circle.move(p->v * game.elapsed.asSeconds());
            if (p->circle.getPosition().x + p->circle.getRadius() < 0.0f || p->circle.getPosition().x - p->circle.getRadius() > window.getSize().x)
                remove_physics_obj(game, i);
            if (p->circle.getPosition().y + p->circle.getRadius() < 0.0f || p->circle.getPosition().y - p->circle.getRadius() > window.getSize().y)
                remove_physics_obj(game, i);
            found++;
        }
        i++;
    }

}

int main()
{
    RenderWindow    window(VideoMode(Vector2u(1600, 900)), "Simple 2D Collision Engine");
    t_game          game;

    add_physics_obj(game, 0, Vector2f(200, 250));
    game.physics_obj[0].v = Vector2f(250.0f, 0);
    add_physics_obj(game, 1, Vector2f(1400, 250));
    game.physics_obj[1].circle.setFillColor(Color::Red);
    game.physics_obj[1].v = Vector2f(-250.0f, 0);
    Clock clock;
    while (window.isOpen())
    {
        handle_events(window, game);
        game.elapsed = clock.restart();
        update_physics(window, game);
        window.clear();
        int found = 0;
        int i = 0;

        while (found < game.obj_count)
        {
            t_physics_obj* p = &game.physics_obj[i];
            if (p->valid)
            {
                if (game.physics_obj[i].valid)
                    window.draw(game.physics_obj[i].circle);
                found++;
            }
            i++;
        }
        if (game.ballselected && game.physics_obj[game.ballindex].valid)
        {
            Vertex line[2];
            line[0].position = game.physics_obj[game.ballindex].circle.getPosition();
            line[0].color = Color::Red;
            line[1].position = window.mapPixelToCoords(Mouse::getPosition(window));
            line[1].color = Color::Red;
            window.draw(line, 2, PrimitiveType::LineStrip);
        }
        window.display();
    }
    return 0;
}
