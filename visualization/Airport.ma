[top]
components : Generator@Generator
components : landing_queue@Queue
components : takeoff_queue@Queue
components : ControlTower@ControlTower
components : Runway@Runway
components : Hangar

Link : out@Generator in@landing_queue
Link : out@landing_queue in_landing@ControlTower
Link : out@takeoff_queue in_takeoff@ControlTower
Link : land@ControlTower land@Runway
Link : takeoff@ControlTower takeoff@Runway
Link : stop_landing@ControlTower stop@landing_queue
Link : stop_takeoff@ControlTower stop@takeoff_queue
Link : done_landing@ControlTower done@landing_queue
Link : done_takeoff@ControlTower done@takeoff_queue
Link : landing_exit@Runway in@Hangar
Link : hangar_exit@Hangar in@takeoff_queue
Link : takeoff_exit@Runway out

[Hangar]
components : Selector@Selector
components : StorageBank

Link : in in@Selector
Link : out1@Selector in1@StorageBank
Link : out2@Selector in2@StorageBank
Link : out3@Selector in3@StorageBank
Link : out4@Selector in4@StorageBank
Link : out@StorageBank hangar_exit

[StorageBank]
components : Bay1@StorageBay
components : Bay2@StorageBay
components : Bay3@StorageBay
components : Bay4@StorageBay
components : Merger@Merger

Link : in1 in@Bay1
Link : in2 in@Bay2
Link : in3 in@Bay3
Link : in4 in@Bay4
Link : out@Bay1 in1@Merger
Link : out@Bay2 in2@Merger
Link : out@Bay3 in3@Merger
Link : out@Bay4 in4@Merger
Link : out@Merger out
