#ifndef __View__H__
#define __View__H__

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <memory>
#include <vector>
#include "Server/TTRServer.h"
#include "TTRController.h"
#include "Wagon.h"

class View : public QGraphicsView {
    Q_OBJECT
    TTRController *Controller;
    ttr::LocalServer *server;

	bool redrawble;

    void create_count_of_players();
    void draw_deck();
    void draw_wagons();
    void draw_map();
    void draw_wagons_count();
    void create_wagon(const WagonBlock &wagon, int owner);
    void draw_active_cards();
    void draw_players_cards();
    void end_game();
    void draw_stations();  // TODO
    void choose_count_of_bots(int count,
                              bool is_server = true,
                              bool is_host = true, const char *c = "");
    void mouseDoubleClickEvent(QMouseEvent *event);
    void host_or_not(bool is_server = true);
    void draw_redraw_button();
    void timed_redraw();
	void disconnected(const char *);
	void draw_whoose_turn();

public:
    void draw_board();
    View(QWidget *parrent = nullptr);

    QGraphicsScene *scene;


public slots:
    void start(bool is_server = true, bool is_host = true, const char *c = "");
    void start_players(int players = 0,
                       bool is_server = true,
                       bool is_host = true, const char *c = "");
    void get_card_from_deck();
	void display_rulles();
    void display_menu();
	void start_port(bool, bool);
};

#endif  //__View__H__
