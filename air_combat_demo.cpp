//����һ����ѧʾ��������CSU,2020������������(C++)����
//    �����ѧʾ����Ի���ImGUI��ͼ�ν����̣����Լ̳кͶ�̬���и�ϰ
//    ��������ˡ������˴����롱�Ĵ���
//        by Lei,   2020-11-20

#include "imgui.h"
#include <vector>
#include <random>//���ڻ�������
#include <string>
#include <cmath>

std::default_random_engine generator;//ȫ�ֱ����������������������Ϊ�����Ϸ��Ҫһ��������ԣ���������˶��ĵз�
std::uniform_int_distribution<int> distribution(0, 100);//ȫ�ֱ������������ɾ��ȷֲ��������

/*���ڱ�����ҵĵ�ǰλ��,���ó�ȫ�ֱ�������֮���FollowPlayerCircle�������ҵ�λ��*/
double player_current_pos_x;
double player_current_pos_y;

//������ĳ����࣬���������඼��Բ�εģ��������ڽ�����ײ��⡣
class AbstractCircle
{
protected:
    ImVec2 _pos;//λ��
    float _radius;//�뾶
    ImDrawList* _pen;//���ڻ��ƵĻ��ʣ���ImGui���еĵײ��ͼ����
public:
    AbstractCircle(ImDrawList* pen, ImVec2 pos, float radius) :_pen(pen), _pos(pos), _radius(radius) {};
    virtual void draw_self() = 0;//�����Լ�
    virtual void live_step(double time) = 0;//�ȹ�һ��ʱ��Ƭ����ÿ��ʱ��Ƭ�п����˶������Ըı䡣
    virtual std::string get_class_name() { return "AbstractCircle"; }//��ö���������

    ImVec2 get_pos() { return _pos; }//��õ�ǰλ��
    float get_radius() { return _radius; }//��ð뾶
};

class PlayerCircle : public AbstractCircle
{
private:
    float _speed = 1;//ÿ���û�����ʱ���ƶ�����
public:
    PlayerCircle(ImDrawList* pen, ImVec2 pos, float radius) :AbstractCircle(pen, pos, radius) {};

    void draw_self() {
        _pen->AddCircleFilled(_pos, _radius, IM_COL32(255, 0, 0, 200));//����һ���򵥵ĺ�ɫʵ��Բ
    }
    void live_step(double time)
    {
        //�����û�������ı�λ��
        if (ImGui::IsKeyPressed(37))//�û��������ͷ����
            _pos.x -= _speed;
        if (ImGui::IsKeyPressed(39))//�û������Ҽ�ͷ����
            _pos.x += _speed;
        if (ImGui::IsKeyPressed(38))//�û������ϼ�ͷ����
            _pos.y -= _speed;
        if (ImGui::IsKeyPressed(40))//�û������¼�ͷ����
            _pos.y += _speed;
    }

    std::string get_class_name() { return "PlayerCircle"; }

    void set_speed(float speed) { _speed = speed; }//�����˶��ٶ�
};

class RandomMoveCircle :public AbstractCircle
{
private:
    float _speed = 1;//ÿ��ʱ��Ƭ���˶�����
public:
    RandomMoveCircle(ImDrawList* pen, ImVec2 pos, float radius) : AbstractCircle(pen, pos, radius) {};

    void draw_self() {
        _pen->AddCircleFilled(_pos, _radius, IM_COL32(0, 0, 0, 200));//����һ���򵥵ĺ�ɫʵ��Բ
    }

    void live_step(double time)
    {
        //���һ�������ˮƽ���򣬲��ظ÷����ƶ�һ������
        bool bLeft = distribution(generator) > 49;
        _pos.x = bLeft ? _pos.x - _speed : _pos.x + _speed;
        //���һ���������ֱ���򣬲��ظ÷����ƶ�һ������
        bool bUp = distribution(generator) > 49;
        _pos.y = bUp ? _pos.y - _speed : _pos.y + _speed;
    }

    std::string get_class_name() { return "RandomMoveCircle"; }

    void set_speed(float speed) { _speed = speed; }
};

/*    �����˴�����  -- ��AbstractCircle������FollowPlayerCircle
*                         ����������ܹ���ÿ��ʱ��Ƭ����ҵĺ�ɫԲ�����ƶ�һ������
*                          Ҫ������С���Ի�ɫ���Բ��ʾ����С�����ͺ���һ����
*                         Ҫ�ܹ������ô�������������С����ƶ�������������
*                          Ҫ�ں���AirCombatWindow����Ϸ��ʼ�����֣����������ָ������������С��
*
*      ������ע�⣺���Ҫ��Ƚ��ѣ�������������������ֵĴ�����������ԡ�
*/

/*�����������������RandomMoveCircle���д������,ΨһҪע�����FollowPlayerCircle���еĻ�ɫС����ƶ���ʽ��ͬ*/
class FollowPlayerCircle :public AbstractCircle/*ע��Ҫpublic�̳�,����ᱨ��*/
{
private:
    float _speed = 1;
public:
    FollowPlayerCircle(ImDrawList* pen, ImVec2 pos, float radius) : AbstractCircle(pen, pos, radius) {};

    void draw_self()
    {
        _pen->AddCircleFilled(_pos, _radius, IM_COL32(255, 255, 0, 200));/*����һ���򵥵Ļ�ɫʵ��Բ,�����IM_COL32(255, 255, 0, 200)�����ǰ����������RGB����(����ɫ+��ɫ+��ɫ������ɫ��ֵ���)*/
    }
    /*
     *����ĳ�Ա�����Ǳ���ʵ�����ѵĲ���,�ѵ�������ε�������ҵ�ǰ��λ��
     *�����live_step�д�����ҵ�x��y����,���ڶ�Ӧ��x��y���궼�Ǻ�����ľ�̬����,û�з���Ȩ��
     *���ڸ���ĸ���AbstractCircle�е�live_step�ǳ��󷽷�,���ܵ��þ�̬����,������δ�����ҵ�ǰ��λ����������Ĺؼ�
     */
    void live_step(double time)
    {
        _pos.x = player_current_pos_x > _pos.x ? _pos.x + 1 : _pos.x - 1;
        _pos.y = player_current_pos_y > _pos.y ? _pos.y + 1 : _pos.y - 1;
    }

    std::string get_class_name() { return "FollowPlayerCircle"; }

    void set_speed(float speed) { _speed = speed; }
};







//ȫ�ֱ���
std::vector<AbstractCircle*> circles;//�������еķ�������󣬰�����ҡ�ע������һ��ȫ�ֱ�����
                                        //���ǻ���main.cpp�ĳ������ʱ�ֹ�delete���б��е�ÿ������
float player_speed = 5.0f;//��ҵ��ƶ��ٶ�
float random_circle_speed = 5.0f;//����˶�С����ٶ�
int num_random_circle = 20;//����˶�С�������
int num_follow_circle = 20;/*������˶�С�������*/
float follow_circle_speed = 5.0f;/*������˶�С����ٶ�*/


void reset_game()//������Ϸ
{
    //�������в���
    player_speed = 5.0f;
    random_circle_speed = 5.0f;

    //�ͷ����з��������
    for (auto o : circles)
    {
        delete o;
    }
    circles.clear();
};

//��Ϸ��������
void AirCombatWindow()
{
    static PlayerCircle* player = NULL;//����ָ����Һ�ɫС���ָ��  
    static bool bPauseGame = false;//����Ƿ�Ҫ��ͣ��Ϸ

    //������Ϸ�ĳ�ʼ��
    ImDrawList* pen = ImGui::GetBackgroundDrawList();//��û��ʶ���
    ImVec2 screen_size = ImGui::GetIO().DisplaySize;
    if (circles.size() == 0)
    {
        //�������
        player = new PlayerCircle(pen, ImVec2(screen_size.x / 2, screen_size.y), 30);
        circles.push_back(player);

        //����10������˶���RandomMoveCircle����
        for (int i = 0; i < num_random_circle; i++)
        {
            float rand_x = distribution(generator) / 100.0 * screen_size.x;

            RandomMoveCircle* obj = new RandomMoveCircle(pen, ImVec2(rand_x, 300), 30);
            circles.push_back(obj);
        }
        /*����10�����û��˶���FollowPlayerCircle����*/
        /*������Է���������Ǹ�forѭ��д,��random��Ӧ�Ĳ��ֻ���follow��Ӧ�Ĳ���*/
        for (int i = 0; i < num_follow_circle; i++)
        {
            float rand_x = distribution(generator) / 100.0 * screen_size.x;

            FollowPlayerCircle* obj = new FollowPlayerCircle(pen, ImVec2(rand_x, 300), 30);
            circles.push_back(obj);
        }
    }

    // ����һ��������崰�ڣ�����������п���������Ϸ��һЩ����
    ImGui::Begin(u8"�ɳ���Χ");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text(u8"���������ǣ�Ҫ���ƺ�ɫԲ������ܵз���������ƶ������ڶ���.\n\n�����ͨ����ͷ�����������·��ĺ�ɫԲ��\n\n���л�����Ը�������ƶ�\n \
           \n ʧ����������ɫԲ�����������������\n �ɹ���������ɫԲ���ƶ����˴��ڶ���");
    ImGui::Separator();
    ImGui::SliderFloat(u8"����ٶ�", &player_speed, 0.0f, 20.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::SliderInt(u8"��ɫ�����������", &num_random_circle, 10, 100);
    ImGui::SliderFloat(u8"��ɫ�������ٶ�", &random_circle_speed, 0.0f, 20.0f);
    ImGui::SliderInt(u8"��ɫ�����������", &num_follow_circle, 10, 100);        /*�����������ӻ�ɫ�����������*/
    ImGui::SliderFloat(u8"��ɫ��������ٶ�", &follow_circle_speed, 0.0f, 20.0f);/*�����������ӻ�ɫ��������ٶ�*/
    ImGui::Checkbox(u8"��ͣ��Ϸ", &bPauseGame);
    if (ImGui::Button(u8"������Ϸ"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    {
        reset_game();
    }

    ImGui::End();

    // ���ݴ����е��û����ã�����ÿ����������ԡ�ע�⣺��������������ʱ�ı䣬������ÿ������ʱ��Ч��
    for (auto obj : circles)
    {
        std::string name = obj->get_class_name();

        if (name == "RandomMoveCircle")
            ((RandomMoveCircle*)obj)->set_speed(random_circle_speed);

        if (name == "PlayerCircle")
            ((PlayerCircle*)obj)->set_speed(player_speed);

        if (name == "FollowPlayerCirle")/*���FollowPlayerCirle����Ӧ�����ò���*/
            ((FollowPlayerCircle*)obj)->set_speed(follow_circle_speed);

    }

    // �����еķ��ж����һ��ʱ��Ƭ�����һ����Լ�
    for (auto obj : circles)
    {
        /*����һ��Ҫ��Ӷ����λ�õĸ���,�Ա���FollowPlayerCirle����live_step���������λ�õķ���*/
        player_current_pos_x = (player->get_pos()).x;
        player_current_pos_y = (player->get_pos()).y;
        if (!bPauseGame)
        {
            obj->live_step(ImGui::GetTime());
        }
            
        obj->draw_self();
    }

    // ������ײ��⣬һ�����ֺ�ɫС����ײ���͵�����Ϸʧ�ܴ��ڣ�����������Ϸ
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
        //��ʾ�Ի�����ʾ��ң���Ϸ�Ѿ�������
        ImGui::OpenPopup(u8"ʧ��!");
        if (ImGui::BeginPopupModal(u8"ʧ��!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"��Ϸ������\n\n��Ϸ�����¿�ʼ\n");
            ImGui::Separator();


            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); reset_game(); bPauseGame = false; }
            ImGui::SetItemDefaultFocus();

            ImGui::EndPopup();
        }
    }


    /*    �����˴�����  -- ����Ϸ�Ƿ�ɹ������жϣ�һ���ɹ����͵����Ի�����ʾ�û�����������Ϸ
    *                          ��Ϸ�ɹ��������ǣ���ɫС��ı�Ե���������ڶ���
    *
    */

    /*�������Ϸ�Ƿ�ɹ����Է���������Ϸ�Ƿ�����Ĳ���
    * ���������Ϸ�Ƿ�����Ĳ��ֿ��Է������Ϊ��������:
    *    һ����������bGameEnd����,���Բ���bGameEnd�Ƿ���½����ж�,�����Ҫ����bGameEnd��ֵ,���轫bPauseGame���ó�true����ֹ����
    *    һ���������������Ϸ����������,�������Ӧ����ʾ��Ϣ�������Ƿ����¿�ʼ��Ϸ
    * ����������Ĺ��ɺ������ԭ��Ҳ�����Ƶ�,Ψһ�Ĳ�ͬ�����ж�������ͬ:��Ϸ�ɹ����ж�����Ϊ��ҵ�С���������С�ڰ뾶
    * ���ǵ�Ҫ���Ĺ���bPauseGame��ԭ
    */
    bool bGameSuccess = false;
    ImVec2 pos_player;/*�洢��ҵ�λ��*/
    float _radius_player;/*�洢���С��İ뾶*/
    pos_player = player->get_pos();
    _radius_player = player->get_radius();
    if (pos_player.y <= _radius_player)
    {
        bGameSuccess = true;
        bPauseGame = true;
    }
    if (bGameSuccess)
    {
        //��ʾ�Ի�����ʾ��ң���Ϸ�ɹ���
        ImGui::OpenPopup(u8"�ɹ�!");
        if (ImGui::BeginPopupModal(u8"�ɹ�!", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"��Ϸ�ɹ���\n\n��Ϸ�����¿�ʼ\n");
            ImGui::Separator();


            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); reset_game(); bPauseGame = false; }
            ImGui::SetItemDefaultFocus();

            ImGui::EndPopup();
        }
    }


}