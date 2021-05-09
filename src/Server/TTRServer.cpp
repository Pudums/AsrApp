#include "Server/TTRServer.h"
#include "TTRController.h"
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
namespace {
    ttr::Rectangle parse_to_grpc_rectangle(const Rectangle& r){
        ttr::Rectangle ans;
        for(auto point: r.points){
            ttr::Point n_point;
            n_point.set_x(point.x);
            n_point.set_y(point.y);
            (*ans.add_points()) = n_point;
        }
        return ans;
    }
}
namespace ttr {

TTRServer::TTRServer(TTRController *c) {
    controller = c;
}

MakeTurnResponse TTRServer::local_make_turn(
    const ::ttr::MakeTurnRequest *request) {
    MakeTurnResponse response;
    response.set_is_success(true);
    if (request->type() == "get board state") {
        *(response.release_current_state()) = local_get_board_state();
    }
    if (request->id().id() != controller->get_current_player_id()) {
        response.set_is_success(false);
        return response;
    }

    if (request->type() == "draw from deck") {  // TODO make global constants
        controller->get_card_from_deck();
    }
    if (request->type() == "draw from active") {
        controller->get_card_from_active(request->active_card_id());
    }
    if (request->type() == "build path") {
        controller->build_path_initialize(request->path_to_build_id());
    }
    if (request->type() == "build station") {
        // TODO
    }
    return response;
}

::grpc::Status TTRServer::get_board_state(::grpc::ServerContext *context,
                                          const ::ttr::Nothing *request,
                                          ::ttr::BoardState *response) {
    *response = local_get_board_state();
    return ::grpc::Status();
}

BoardState TTRServer::local_get_board_state() {

    ::ttr::BoardState state;
    Board board;
    Deck deck;
    std::vector<::Path> all_paths = controller->get_all_paths();
    for (auto & path : all_paths) {
        ttr::Path n_path;
        n_path.set_color(path.color);
        n_path.set_finish(path.finish);
        n_path.set_start(path.start);
        n_path.set_number_of_colored_wagons(path.number_of_colored_wagons);
        n_path.set_number_of_locomotives(path.number_of_locomotives);
        n_path.set_is_tunnel(path.is_tunnel);
        n_path.set_owner(path.owner);
        for(const auto& w: path.wagon_blocks){
            ttr::WagonBlock n_block;
            n_block.set_id(w.id);
            n_block.set_color(w.color);
            n_block.set_allocated_coords(new ttr::Rectangle());
            *(n_block.mutable_coords()) = parse_to_grpc_rectangle(w.coords);
            *(n_path.add_wagon_blocks()) = n_block;
        }
        n_path.set_length(path.length);
        *(board.add_paths()) = n_path;
    }

    std::vector<WagonCard> active_cards = controller->get_active_cards();
    for (auto & active_card : active_cards) {
        Card n_card;
        n_card.set_type(::ttr::Card_Type_Wagon);
        n_card.set_allocated_wagon_info(new ::ttr::Wagon());
        n_card.release_wagon_info()->set_color(active_card.color);
        *(deck.add_cards_on_table()) = n_card;
    }
    *state.mutable_board_state() = board;
    *state.mutable_deck_state() = deck;
    state.set_allocated_all_players(new Players());
    for (int i = 0; i < controller->get_players().size(); i++) {
        auto player = controller->get_players()[i];
        ttr::Player p;
        p.set_current_score(controller->get_results()[player.id]);
        p.set_routes_num(player.active_routes.size());
        p.set_wagons_left(player.number_of_wagons_left);
        *(state.mutable_all_players()->add_all_players()) = p;
    }
    return state;
}

::grpc::Status TTRServer::make_turn(::grpc::ServerContext *context,
                                    const ::ttr::MakeTurnRequest *request,
                                    ::ttr::MakeTurnResponse *response) {
    *response = local_make_turn(request);
    return ::grpc::Status();
}
::grpc::Status TTRServer::get_score(::grpc::ServerContext *context,
                                    const Nothing *request,
                                    ::ttr::INT_ARRAY *result) {
    //TODO
    return grpc::Status();
}
::grpc::Status TTRServer::get_player_id(::grpc::ServerContext *context,
                                        const ::ttr::Nothing *request,
                                        ::ttr::PlayerID *response) {
    static int last_id = 0;
    if(!controller->is_game_started()){
        response->set_id(last_id);
        last_id++;
    }else{
        response->set_id(controller->get_current_player_id());
    }
    return ::grpc::Status::OK;
}
::grpc::Status TTRServer::start_game(::grpc::ServerContext *context,
                                     const ::ttr::Nothing *request,
                                     ::ttr::Nothing *result) {
    controller->start_game_server();
    return ::grpc::Status::OK;
}

LocalServer::LocalServer(TTRController *c) : service(TTRServer(c)) {
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    server = builder.BuildAndStart();
}

void LocalServer::runServer() {
    server->Wait();
}

void LocalServer::terminate() {
    server->Shutdown();
}

void RunServer(LocalServer *serv, bool needRun) {
    if (needRun) {
        std::cout << "Server started!!!";
        serv->runServer();
    }
}

}  // namespace ttr
