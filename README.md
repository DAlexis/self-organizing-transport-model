Self-organizing transport modeller
==================================


Self-organizing transoprt modeller (SOTM) is a library for modelling different physical processes based on dynamic graphs in space. The project was created to model lightning discharge but may be applied to other physical processes. 

Now this code works well for lightning initiation process modelling and streamer-to-leader transition. Lightning modelling is the only development focus for now.

If you want to use my code for some reason, you are free to communicate over `AlekseyABulatov@yandex.ru`

# Some notes
Modelling result is not determenistic for now, so different runs give different results. There are two sources of undeterminism:

* Multithreading case: random numbers are generated from one source, so it is a kind of data race

* Even for single thread it is undeterministic because of constructions like ` std::set<Node*> m_nodes`, and nodes are allocated with new with random memory addresses in general case. This cause that iterating order is random too (data is sorted by address). So order of random numbers generated for simulation is undeterministic even with concrete seed.