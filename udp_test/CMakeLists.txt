add_executable(udp_client client.c)
target_link_libraries(udp_client pthread)

add_executable(udp_server server.c)
target_link_libraries(udp_server pthread)