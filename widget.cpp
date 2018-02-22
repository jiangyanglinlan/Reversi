#include "widget.h"
#include "ui_widget.h"
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

#define cout qDebug()


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
    {
        ui->setupUi(this);

        //棋盘左上角坐标
        Start = QPoint(385, 36);

        //棋盘右下角坐标
        End = QPoint(1011, 666);

        GridW = (End.x() - Start.x()) / 8;
        GridH = (End.y() - Start.y()) / 8;

        InitReversi();

        connect(&mTimer, &QTimer::timeout, this, &Widget::AiPlay);

        connect(&LeftTimer, &QTimer::timeout,
        [=]() mutable
        {
            TimeNum--;
            ui->Time->display(TimeNum);

            //倒计时结束
            if(!TimeNum)
            {
                TimeNum = 16;
                this->ChangeRole();
            }
        }

        );
     }

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *)
{
    //执行绘图操作
    QPainter p(this);

    p.drawPixmap(this->rect(), QPixmap("://images/chessboard.png"));

    //实现下棋子
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(Reversi[i][j] == Black)
            {
                //下黑棋
                p.drawPixmap(Start.x() + i * GridW, Start.y() + j * GridH, GridW - 2, GridH - 2, QPixmap("://images/heiqi.tga"));

            }
            else if(Reversi[i][j] == White)
            {
                //下白棋
                p.drawPixmap(Start.x() + i * GridW, Start.y() + j * GridH, GridW - 2, GridH - 2, QPixmap("://images/baiqi.tga"));
            }
        }
    }
}

//鼠标点击事件
void Widget::mousePressEvent(QMouseEvent *qme)
{
    //处理鼠标左键点击事件
    if(qme->button() & Qt::LeftButton)
    {
       p = qme->globalPos() - this->frameGeometry().topLeft();
    }

    //cout << "qme = " << qme->pos();

    int x = qme->x();
    int y = qme->y();
    int i = 0;
    int j = 0;

    if(x >= Start.x() && x <= (Start.x() + 8 * GridW) && y >= Start.y() && y <= (Start.y() + 8 * GridH))
    {
        i = (qme->x() - Start.x()) / GridW;
        j = (qme->y() - Start.y()) / GridH;

        //Reversi[i][j] = Role;
        if(JudgeRule(i, j, Reversi, Role) > 0)
        {
            flag++;
            LeftTimer.start();
            //改变角色
            this->ChangeRole();

            CanDrop();

            //更新绘图
            update();
        }
    }
}

//实现鼠标拖动窗口
void Widget::mouseMoveEvent(QMouseEvent *qme)
{
    if(qme->button() & Qt::LeftButton)
    {
        move(qme->globalPos() - p);
    }
}

void Widget::on_Exit_clicked()
{
    this->close();
}

//初始化棋盘状态
void Widget::InitReversi()
{
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            //所有棋盘格子设为空
            Reversi[i][j] = Empty;
        }
    }

    //游戏初始的四颗棋子
    Reversi[3][3] = Black;
    Reversi[4][4] = Black;
    Reversi[3][4] = White;
    Reversi[4][3] = White;

    Role = Black; //默认黑子先下

    ui->Black->show();
    ui->White->hide();

    //黑棋和白棋的计数
    ui->BlackCount->display(2);
    ui->WhiteCount->display(2);

    //将判断输赢的Label隐藏起来
    ui->YouWin->hide();
    ui->YouLose->hide();
    ui->Draw->hide();

    //倒计时为15秒
    TimeNum = 15;
    ui->Time->display(TimeNum);
    if(LeftTimer.isActive() == false)
    {
        //定时器
        this->LeftTimer.start(1000);
    }
}

//切换角色
void Widget::ChangeRole()
{
    if(Role == Black)
    {
        Role = White;
        TimeNum = 16;
        ui->Black->hide();
        ui->White->show();
    }
    else
    {
        Role = Black;
        TimeNum = 16;
        ui->Black->show();
        ui->White->hide();
    }

    if(Role == White) //机器下子
    {
        this->mTimer.start(1000); //毫秒为单位
    }

    //统计个数
    ShowCount();

}

//统计个数
void Widget::ShowCount()
{
    int b = 0;
    int w = 0;
    for(int i =0; i < 8; ++i)
    {
        for(int j =0; j < 8; ++j)
        {
            if(Reversi[i][j] == Black)
            {
                b++;
            }
            else if(Reversi[i][j] == White)
            {
                w++;
            }
        }
    }

    ui->BlackCount->display(b);
    ui->WhiteCount->display(w);

    //输赢判断
    //黑子、白子都不能吃子，说明游戏结束
    for(int i =0; i < 8; ++i)
    {
        for(int j =0; j < 8; ++j)
        {
            if(JudgeRule(i, j, Reversi, Black, false) > 0 || JudgeRule(i, j, Reversi, White, false) > 0)
            {
                return;
            }
        }
    }

    //指向到这步，说明游戏结束
    if(w > b)
    {
        //cout << "白子赢";
        this->LeftTimer.stop();
        ui->YouLose->show();
    }
    else if(w < b)
    {
        //cout << "黑子赢";
        this->LeftTimer.stop();
        ui->YouWin->show();
    }
    else
    {
        //cout << "平局";
        this->LeftTimer.stop();
        ui->Draw->show();
    }
}

//吃子规则的参数：棋盘数组坐标位置(x y)、棋子状态数组(reversi)、棋子的当前角色
//eatChess默认为true, 横着或竖着的格数(gridNum)默认值为8
int Widget::JudgeRule(int x, int y, void *reversi, ReversiStatus currentRole, bool eatChess, int gridNum)
{

    //棋盘的八个方向
    int dir[8][2]={{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1},{1,1}};

    //保存棋盘数组坐标位置
    int tmpX = x, tmpY = y;

    //数据初始化
    int i = 0, eatNum = 0;

    //自定义类型
    typedef int (*P)[gridNum];

    //类型转换
    P chessBoard = P(reversi);

    //判断此坐标格子是否为空
    if(chessBoard[tmpX][tmpY] != Empty)
        return 0;

    //棋盘的8个方向
    for(i = 0 ; i <8; i++)
    {
        //判断相邻的棋子
        tmpX += dir[i][0]; tmpY += dir[i][1];
        //如果没有出界，且相邻棋子是对方棋子，才有吃子的可能．

        if((tmpX < gridNum && tmpX >=0 && tmpY < gridNum && tmpY >= 0)&& (chessBoard[tmpX][tmpY] != currentRole) && (chessBoard[tmpX][tmpY] != Empty))
        {
            //继续向前判断
            tmpX += dir[i][0];
            tmpY += dir[i][1];
            while(tmpX < gridNum && tmpX >=0 && tmpY < gridNum && tmpY >= 0)
            {
                //遇到空位跳出
                if(chessBoard[tmpX][tmpY] == Empty)
                {
                    break;
                }

                //找到自己的棋子,代表可以吃子
                if(chessBoard[tmpX][tmpY] == currentRole)
                {
                    //确定吃子
                    if(eatChess == true)
                    {
                        //开始点标志为自己的棋子
                        chessBoard[x][y] = currentRole;

                        //后退一步
                        tmpX -= dir[i][0];
                        tmpY -= dir[i][1];

                        //只要没有回到开始的位置就执行
                        while((tmpX != x )||(tmpY != y))
                        {
                            //标志为自己的棋子
                            chessBoard[tmpX][tmpY] = currentRole;
                            //继续后退一步
                            tmpX -= dir[i][0];
                            tmpY -= dir[i][1];
                            //吃子计数+1
                            eatNum++;
                        }
                    }
                    //不吃子,只判断这个格子能不能吃子
                    else
                    {
                        //后退一步
                        tmpX -= dir[i][0];
                        tmpY -= dir[i][1];
                        //只计算可以吃子的个数
                        while((tmpX != x )||(tmpY != y))
                        {
                            //继续后退一步
                            tmpX -= dir[i][0];
                            tmpY -= dir[i][1];
                            eatNum++;
                        }
                    }
                    //跳出循环
                    break;
                }
                // 没有找到自己的棋子，就向前走一步
                tmpX += dir[i][0]; tmpY += dir[i][1];
            }
        }
        // 如果这个方向不能吃子，就换一个方向
        tmpX = x;
        tmpY = y;
    }
    //cout << "eatNum = " << eatNum << endl;

    //返回能吃子的个数
    return eatNum;
}

//电脑下棋
void Widget::AiPlay()
{
    this->mTimer.stop();

    int max = 0;
    int x, y;

    if(JudgeRule(0, 0, Reversi, White, true) > 0)
    {
        //改变角色
        this->ChangeRole();

        CanDrop();

        //更新绘图
        update();

        return ;
    }
    else if(JudgeRule(0, 7, Reversi, White, true) > 0)
    {
        //改变角色
        this->ChangeRole();

        CanDrop();

        //更新绘图
        update();

        return ;
    }
    else if(JudgeRule(7, 0, Reversi, White, true) > 0)
    {
        //改变角色
        this->ChangeRole();

        CanDrop();

        //更新绘图
        update();

        return ;
    }
    else if(JudgeRule(7, 7, Reversi, White, true) > 0)
    {
        //改变角色
        this->ChangeRole();

        CanDrop();

        //更新绘图
        update();

        return ;
    }
    //找出利益最大的下子坐标
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(JudgeRule(i, j, Reversi, White, false))
            {
                if(JudgeRule(i, j, Reversi, White, false) > max)
                {
                    max = JudgeRule(i, j, Reversi, White, false);
                    x = i;
                    y = j;
                }
            }
        }
    }

    //不能下子了
    if(max == 0)
    {
        this->ChangeRole();
        return;
    }
    else
    {
        if(JudgeRule(x, y, Reversi, White) > 0)
        {
            //改变角色
            this->ChangeRole();

            //更新绘图
            update();
        }
    }
}

void Widget::on_Pause_clicked()
{
    flag++;
    if(flag % 2 == 1)
    {
        LeftTimer.stop();
    }
    else
    {
        LeftTimer.start();
    }
}

void Widget::CanDrop()
{
    int flag2 = 1;
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            //如果有一个点能下子,则标记变量flag2 = 0;
            if(JudgeRule(i, j, Reversi, Role, false) > 0)
            {
                flag2 = 0;
            }
        }
    }
    //如果当前角色不能下子
    if(flag2)
    {
        this->ChangeRole();
    }
}


void Widget::on_Start_clicked()
{
    //初始化棋盘状态
    InitReversi();

    this->update();
}

