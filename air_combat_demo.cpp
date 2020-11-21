//这是一个教学示范，属于CSU,2020秋，《面向对象编程(C++)》。
//    这个教学示范针对基于ImGUI的图形界面编程，并对继承和多态进行复习
//    请填充标记了“请填充此处代码”的代码
//        by Lei,   2020-11-20

#include "imgui.h"
#include <vector>
#include <random>//用于获得随机数
#include <string>
#include <cmath>

std::default_random_engine generator;//全局变量，用于生成随机数，因为这个游戏需要一定的随机性，比如随机运动的敌方
std::uniform_int_distribution<int> distribution(0, 100);//全局变量，用于生成均匀分布的随机数

/*用于保存玩家的当前位置,设置成全局变量方便之后的FollowPlayerCircle类访问玩家的位置*/
double player_current_pos_x;
double player_current_pos_y;

//飞行物的抽象类，所有派生类都是圆形的，这样便于进行碰撞检测。
class AbstractCircle
{
protected:
    ImVec2 _pos;//位置
    float _radius;//半径
    ImDrawList* _pen;//用于绘制的画笔，是ImGui库中的底层绘图对象。
public:
    AbstractCircle(ImDrawList* pen, ImVec2 pos, float radius) :_pen(pen), _pos(pos), _radius(radius) {};
    virtual void draw_self() = 0;//绘制自己
    virtual void live_step(double time) = 0;//度过一个时间片。在每个时间片中可以运动、可以改变。
    virtual std::string get_class_name() { return "AbstractCircle"; }//获得对象的类别名

    static void reset_pos(std::vector<AbstractCircle*>& obj);/*声明让黑色球和黄色球互不重叠函数*/

    ImVec2 get_pos() { return _pos; }//获得当前位置
    float get_radius() { return _radius; }//获得半径
};

class PlayerCircle : public AbstractCircle
{
private:
    float _speed = 1;//每次用户按键时的移动步长
public:
    PlayerCircle(ImDrawList* pen, ImVec2 pos, float radius) :AbstractCircle(pen, pos, radius) {};

    void draw_self() {
        _pen->AddCircleFilled(_pos, _radius, IM_COL32(255, 0, 0, 200));//就是一个简单的红色实心圆
    }
    void live_step(double time)
    {
        //根据用户的输入改变位置
        if (ImGui::IsKeyPressed(37))//用户按下左箭头按键
            _pos.x -= _speed;
        if (ImGui::IsKeyPressed(39))//用户按下右箭头按键
            _pos.x += _speed;
        if (ImGui::IsKeyPressed(38))//用户按下上箭头按键
            _pos.y -= _speed;
        if (ImGui::IsKeyPressed(40))//用户按下下箭头按键
            _pos.y += _speed;
    }

    std::string get_class_name() { return "PlayerCircle"; }

    void set_speed(float speed) { _speed = speed; }//设置运动速度
};

class RandomMoveCircle :public AbstractCircle
{
private:
    float _speed = 1;//每个时间片的运动步长
public:
    RandomMoveCircle(ImDrawList* pen, ImVec2 pos, float radius) : AbstractCircle(pen, pos, radius) {};

    void draw_self() {
        _pen->AddCircleFilled(_pos, _radius, IM_COL32(0, 0, 0, 200));//就是一个简单的黑色实心圆
    }

    void live_step(double time)
    {
        //获得一个随机的水平方向，并沿该方向移动一个步长
        bool bLeft = distribution(generator) > 49;
        _pos.x = bLeft ? _pos.x - _speed : _pos.x + _speed;
        //获得一个随机的竖直方向，并沿该方向移动一个步长
        bool bUp = distribution(generator) > 49;
        _pos.y = bUp ? _pos.y - _speed : _pos.y + _speed;
    }

    std::string get_class_name() { return "RandomMoveCircle"; }

    void set_speed(float speed) { _speed = speed; }
};

/*    请填充此处代码  -- 从AbstractCircle派生类FollowPlayerCircle
*                         这个派生类能够在每个时间片朝玩家的红色圆球方向移动一个步长
*                          要求这类小球以黄色填充圆显示，大小与红球和黑球一样。
*                         要能够在设置窗口中设置这类小球的移动步长和数量。
*                          要在函数AirCombatWindow的游戏初始化部分，创建并添加指定数量的这类小球
*
*      ！！！注意：这个要求比较难，建议你填充完其他部分的代码后再来尝试。
*/

/*下面的这个类仿照上面RandomMoveCircle类的写法即可,唯一要注意的是FollowPlayerCircle类中的黄色小球的移动方式不同*/
class FollowPlayerCircle :public AbstractCircle/*注意要public继承,否则会报错*/
{
private:
    float _speed = 1;
public:
    FollowPlayerCircle(ImDrawList* pen, ImVec2 pos, float radius) : AbstractCircle(pen, pos, radius) {};

    void draw_self()
    {
        _pen->AddCircleFilled(_pos, _radius, IM_COL32(255, 255, 0, 200));/*就是一个简单的黄色实心圆,这里的IM_COL32(255, 255, 0, 200)里面的前三个参数是RGB编码(即红色+绿色+蓝色三个颜色的值混合)*/
    }
    /*
     *下面的成员函数是本次实验最难的部分,难点在于如何调访问玩家当前的位置
     *如果在live_step中传入玩家的x和y坐标,由于对应的x和y坐标都是函数里的静态变量,没有访问权限
     *且在该类的父类AbstractCircle中的live_step是抽象方法,不能调用静态变量,所以如何传入玩家当前的位置则是问题的关键
     */
    void live_step(double time)
    {
        _pos.x = player_current_pos_x > _pos.x ? _pos.x + 1 : _pos.x - 1;
        _pos.y = player_current_pos_y > _pos.y ? _pos.y + 1 : _pos.y - 1;
    }

    std::string get_class_name() { return "FollowPlayerCircle"; }

    void set_speed(float speed) { _speed = speed; }
};







//全局变量
std::vector<AbstractCircle*> circles;//保存所有的飞行物对象，包括玩家。注意这是一个全局变量，
                                        //我们会在main.cpp的程序结束时手工delete该列表中的每个对象。
float player_speed = 5.0f;//玩家的移动速度
float random_circle_speed = 5.0f;//随机运动小球的速度
int num_random_circle = 20;//随机运动小球的数量
int num_follow_circle = 20;/*朝玩家运动小球的数量*/
float follow_circle_speed = 5.0f;/*朝玩家运动小球的速度*/


void reset_game()//重置游戏
{
    //重置所有参数
    player_speed = 5.0f;
    random_circle_speed = 5.0f;

    //释放所有飞行物对象
    for (auto o : circles)
    {
        delete o;
    }
    circles.clear();
};

/*让球与球之间(不包括红球)互不重叠,原理为如果两球的距离小于两者的半径之和则各退一个半径*/
void AbstractCircle::reset_pos(std::vector<AbstractCircle*>& obj)
{
    for (std::vector<AbstractCircle*>::iterator i = obj.begin() + 1; obj.end() != i; i++)/*这里面的std::vector<AbstractCircle*>::iterator i也可以换成auto i*/
    {
        for (auto j = i + 1; obj.end() != j; j++)
        {
            float x1 = (*i)->get_pos().x, x2 = (*j)->get_pos().x;
            float y1 = (*i)->get_pos().y, y2 = (*j)->get_pos().y;
            float radius1 = (*i)->get_radius();
            float radius2 = (*j)->get_radius();
            float distence = pow(x1 - x2, 2) + pow(y1 - y2, 2) - pow(radius1 + radius2, 2);

            if (distence < 0)
            {
                if (x1 < x2)
                {
                    (*i)->_pos.x -= radius1;
                    (*j)->_pos.x += radius2;
                }
                else
                {
                    (*i)->_pos.x += radius1;
                    (*j)->_pos.x -= radius2;
                }
                if (y1 < y2)
                {
                    (*i)->_pos.y -= radius1;
                    (*j)->_pos.y += radius2;
                }
                else
                {
                    (*i)->_pos.y += radius1;
                    (*j)->_pos.y -= radius2;
                }
            }
        }
    }
}



//游戏的主函数
void AirCombatWindow()
{
    static PlayerCircle* player = NULL;//保存指向玩家红色小球的指针  
    static bool bPauseGame = false;//标记是否要暂停游戏

    //进行游戏的初始化
    ImDrawList* pen = ImGui::GetBackgroundDrawList();//获得画笔对象
    ImVec2 screen_size = ImGui::GetIO().DisplaySize;
    if (circles.size() == 0)
    {
        //创建玩家
        player = new PlayerCircle(pen, ImVec2(screen_size.x / 2, screen_size.y), 30);
        circles.push_back(player);

        //创建10个随机运动的RandomMoveCircle对象
        for (int i = 0; i < num_random_circle; i++)
        {
            float rand_x = distribution(generator) / 100.0 * screen_size.x;

            RandomMoveCircle* obj = new RandomMoveCircle(pen, ImVec2(rand_x, 300), 30);
            circles.push_back(obj);
        }
        /*创建10个朝用户运动的FollowPlayerCircle对象*/
        /*这个可以仿照上面的那个for循环写,把random相应的部分换成follow对应的部分*/
        for (int i = 0; i < num_follow_circle; i++)
        {
            float rand_x = distribution(generator) / 100.0 * screen_size.x;

            FollowPlayerCircle* obj = new FollowPlayerCircle(pen, ImVec2(rand_x, 300), 30);
            circles.push_back(obj);
        }
    }

    // 创建一个控制面板窗口，在这个窗口中可以设置游戏的一些参数
    ImGui::Begin(u8"飞出重围");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text(u8"你的任务就是：要控制红色圆球来躲避敌方飞行物，并移动到窗口顶部.\n\n你可以通过箭头按键来控制下方的红色圆球。\n\n其中黄球可以跟随红球移动\n \
           \n 失败条件：红色圆球碰到了其他飞行物；\n 成功条件：红色圆球移动到了窗口顶部");
    ImGui::Separator();
    ImGui::SliderFloat(u8"玩家速度", &player_speed, 0.0f, 20.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderInt(u8"黑色飞行物的数量", &num_random_circle, 10, 100);
    ImGui::SliderFloat(u8"黑色飞行物速度", &random_circle_speed, 0.0f, 20.0f);
    ImGui::SliderInt(u8"黄色飞行物的数量", &num_follow_circle, 10, 100);        /*在设置面板添加黄色飞行物的数量*/
    ImGui::SliderFloat(u8"黄色飞行物的速度", &follow_circle_speed, 0.0f, 20.0f);/*在设置面板添加黄色飞行物的速度*/
    ImGui::Checkbox(u8"暂停游戏", &bPauseGame);
    if (ImGui::Button(u8"重置游戏"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    {
        reset_game();
    }

    ImGui::End();

    // 根据窗口中的用户设置，调整每个对象的属性。注意：对象数量不能随时改变，而是在每次重置时生效。
    for (auto obj : circles)
    {
        std::string name = obj->get_class_name();

        if (name == "RandomMoveCircle")
            ((RandomMoveCircle*)obj)->set_speed(random_circle_speed);

        if (name == "PlayerCircle")
            ((PlayerCircle*)obj)->set_speed(player_speed);

        if (name == "FollowPlayerCirle")/*添加FollowPlayerCirle类相应的重置操作*/
            ((FollowPlayerCircle*)obj)->set_speed(follow_circle_speed);

    }

    // 让所有的飞行对象活一个时间片，并且绘制自己
    for (auto obj : circles)
    {
        /*这里一定要添加对玩家位置的更新,以便于FollowPlayerCirle类中live_step方法对玩家位置的访问*/
        player_current_pos_x = (player->get_pos()).x;
        player_current_pos_y = (player->get_pos()).y;
        if (!bPauseGame)
        {
            obj->live_step(ImGui::GetTime());
        }
        obj->draw_self();
    }

    AbstractCircle::reset_pos(circles);

    // 进行碰撞检测，一旦发现红色小球被碰撞，就弹出游戏失败窗口，并且重置游戏
    bool bGameEnd = false;
    for (auto obj : circles)
    {
        if (obj->get_class_name() != "PlayerCircle"&& obj->get_class_name() != "FollowPlayerCircle")
        {
            float dist_required = obj->get_radius() + player->get_radius();

            ImVec2 pos1, pos2;
            pos1 = obj->get_pos();
            pos2 = player->get_pos();
            float dist_real = sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));

            if (dist_real < dist_required)
            {
                bGameEnd = true;
                bPauseGame = true;
                break;
            }
        }
    }
    if (bGameEnd)
    {
        //显示对话框，提示玩家，游戏已经结束。
        ImGui::OpenPopup(u8"失败!");
        if (ImGui::BeginPopupModal(u8"失败!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"游戏结束了\n\n游戏将重新开始\n");
            ImGui::Separator();


            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); reset_game(); bPauseGame = false; }
            ImGui::SetItemDefaultFocus();

            ImGui::EndPopup();
        }
    }


    /*    请填充此处代码  -- 对游戏是否成功进行判断，一旦成功，就弹出对话框提示用户，并重置游戏
    *                          游戏成功的条件是：红色小球的边缘触碰到窗口顶部
    *
    */

    /*下面的游戏是否成功可以仿照上面游戏是否结束的部分
    * 从上面的游戏是否结束的部分可以发现其分为两个部分:
    *    一部分是设置bGameEnd参数,并对参数bGameEnd是否更新进行判断,如果需要更新bGameEnd的值,则需将bPauseGame设置成true即中止程序
    *    一部分是如果满足游戏结束的条件,则输出相应的提示信息并重置是否重新开始游戏
    * 发现了上面的规律后下面的原理也是类似的,唯一的不同点是判断条件不同:游戏成功的判断条件为玩家的小球的纵坐标小于半径
    * 最后记得要将改过的bPauseGame还原
    */
    bool bGameSuccess = false;
    ImVec2 pos_player;/*存储玩家的位置*/
    float _radius_player;/*存储玩家小球的半径*/
    pos_player = player->get_pos();
    _radius_player = player->get_radius();
    if (pos_player.y <= _radius_player)
    {
        bGameSuccess = true;
        bPauseGame = true;
    }
    if (bGameSuccess)
    {
        //显示对话框，提示玩家，游戏成功。
        ImGui::OpenPopup(u8"成功!");
        if (ImGui::BeginPopupModal(u8"成功!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"游戏成功了\n\n游戏将重新开始\n");
            ImGui::Separator();


            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); reset_game(); bPauseGame = false; }
            ImGui::SetItemDefaultFocus();

            ImGui::EndPopup();
        }
    }
}
