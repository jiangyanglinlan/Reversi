#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>

namespace Ui {
    class Widget;
}


//窗口类
class Widget : public QWidget
{
    Q_OBJECT

    public:
        explicit Widget(QWidget *parent = 0);
        ~Widget();

        //枚举变量, 标记黑白棋棋盘点的状态
        enum ReversiStatus{
            Empty,
            Black,
            White
        };

        // 估值表
        int xy_value[8][8] = {
            {90, -60, 10, 10, 10, 10, -60, 90},
            {-60, -80, 5, 5, 5, 5, -80, -60},
            {10, 5, 1, 1, 1, 1, 5, 10},
            {10, 5, 1, 1, 1, 1, 5, 10},
            {10, 5, 1, 1, 1, 1, 5, 10},
            {10, 5, 1, 1, 1, 1, 5, 10},
            {-60, -80, 5, 5, 5, 5, -80, -60},
            {90, -60, 10, 10, 10, 10, -60, 90}
        };

        //初始化棋盘所有坐标状态
        void InitReversi();

        //切换下棋的角色
        void ChangeRole();

        //统计棋子个数
        void ShowCount();

        //判断是否能吃子
        int JudgeRule(int x, int y, void *reversi, ReversiStatus currentRole, bool eatChess = true, int gridNum = 8);

        //电脑下棋
        void AiPlay();

        //实现暂停的变量flag
        int flag = 0;

        //判断是否有子可下
        void CanDrop();

    private:
        Ui::Widget *ui;

        //保存坐标
        QPoint p;

        //棋盘左上角的坐标
        QPoint Start;

        //棋盘右上角的坐标
        QPoint End;

        //棋盘格子的宽度
        int GridW;

        //棋盘格子的高度
        int GridH;

        //表示棋盘的二维数组
        int Reversi[8][8];

        //下棋的角色
        ReversiStatus Role;

        //倒计时
        int TimeNum;
        QTimer LeftTimer;

        //定时器
        QTimer mTimer;

    protected:
        //绘制棋盘和棋子
        void paintEvent(QPaintEvent *);

        //鼠标点击事件
        void mousePressEvent(QMouseEvent *);

        //实现鼠标拖动窗口
        void mouseMoveEvent(QMouseEvent *);

private slots:
        void on_Exit_clicked();
        void on_Start_clicked();
        //void on_toolButton_clicked();
        void on_Pause_clicked();
};


#endif // WIDGET_H
