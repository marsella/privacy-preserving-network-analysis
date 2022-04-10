import matplotlib.pyplot as plt
import numpy as np
import parse_logs 

def make_csv(data, label):
    with open("../../../figures/scalemamba-%s.csv"%label, "w") as f:
        f.write("n %sruntime %sram\n"%(label, label))
        for dat in data:
            f.write("{},{},{}\n".format(
                dat.n_banks,
                max(dat.runtime_time),
                max(dat.runtime_ram)/1000000))

def make_fixed_csv(endata):
    with open("../../../figures/scalemamba-fixed.csv", "w") as f:
        f.write("n ENruntime ENram \n")
        for en in endata:
            f.write("{},{},{}\n".format(
                en.n_banks,
                max(en.runtime_time),
                max(en.runtime_ram)/1000000))

# takes a list of [Bank]s and corresponding ["labels"]
def plot_runtime(data, labels,figname):
    WIDTH = .3
    ax = plt.axes()

    for idx,(dataset,lab) in enumerate(zip(data,labels)):
        xs = [d.n_banks + idx*WIDTH for d in dataset]
        ys = [max(d.runtime_time) for d in dataset]
        plt.scatter(xs, ys, label=lab)

        plt.plot(range(2,21), np.poly1d(np.polyfit(xs, ys, 2))(range(2,21)),
            '--', label="expected runtime")

        for d in dataset:
            if d.mac_fail:
                offset = idx*WIDTH 
                ax.text(d.n_banks + offset, max(d.runtime_time)+5, 'F')

    # add labels for timing context and failures
    ax.hlines(y=60, xmin=2, xmax=20,linestyle='dashed',color='gray')
    ax.text(15.5,260,'1 minute',color='gray')
    ax.hlines(y=3600, xmin=2, xmax=20,linestyle='dashed',color='gray')
    ax.text(15.5,3800,'1 hour',color='gray')
    ax.hlines(y=28800, xmin=2, xmax=20,linestyle='dashed',color='gray')
    ax.text(15.5,29000,'8 hours',color='gray')

    # add plot info
    plt.xticks(np.arange(2,22,2))
    #plt.title("Timing for EN algorithm")
    plt.xlabel("number of participants")
    plt.ylabel("time (seconds)")
    plt.legend(framealpha=1)

    plt.savefig(figname)
    plt.clf()



# plot runtime ram usage
def plot_ram_usage(data, labels,figname):
    WIDTH = .3
    ax = plt.axes()
    
    for idx,(dataset,lab) in enumerate(zip(data,labels)):
        xs = [d.n_banks + idx*WIDTH for d in dataset]
        ys = [max(d.runtime_ram)/1000000 for d in dataset]
        plt.scatter(xs, ys, label = lab)

        plt.plot(range(2,21), np.poly1d(np.polyfit(xs, ys, 1))(range(2,21)),
            '--', label="expected RAM use")

        for d in dataset:
            if d.mac_fail:
                offset = WIDTH if d.offline else 0
                ax.text(d.n_banks + offset, max(d.runtime_ram)/1000000 + .3, 'F')


    plt.xticks(np.arange(2,22,2))
    #plt.title("peak RAM usage per player for EN algorithm")
    plt.xlabel("number of participants")
    plt.ylabel("peak RAM usage (gB)")
    plt.legend()

    plt.savefig(figname)
    plt.clf()


def plot_rounds(data):
    # plot number of rounds (same for all)
    # todo verify this is true
    banks_by_rounds = [(d.n_banks, d.rounds) for d in data]
    
    num_banks, num_rounds = zip(*banks_by_rounds)
    
    plt.plot(num_banks, num_rounds, 'o')
    plt.xlabel("number of banks")
    plt.ylabel("rounds of communication")
    plt.savefig("rounds.png")
    plt.clf()

if __name__ == "__main__":

    egjdata = []
    endata = []
    for parties in range(2,13,2):
        try:
            log = parse_logs.parse_egj_logs(parties) 
            egjdata.append(log)
        except:
            print("no data for egj @ ", parties)

        try:
            log = parse_logs.parse_logs(parties, 0, 0)
            endata.append(log)
        except:
            print("no data for en @ ", parties)

    plot_runtime([egjdata, endata],["EGJ", "EN"], "sm-time.png")
    plot_ram_usage([egjdata, endata],["EGJ", "EN"], "sm-ram.png")

    make_csv(endata, "EN")
    make_csv(egjdata, "EGJ")

    fixdata = []
    for parties in range(2,11,2):
        log = parse_logs.parse_fixed_logs(parties)
        fixdata.append(log)
    make_fixed_csv(fixdata)

    

    """
    endata = []
    small_batch_data = []

    for parties in range(2,9,2):
        for off,sacr in [(0,0),(1,1),(0,1)]:
            log = parse_logs.parse_logs(parties, off, sacr)
            endata.append(log)

            small_batch_data.append(parse_logs.parse_logs(parties,off,sacr,HOME="./batch_logs/"))


    # plot runtime information
    all_fake_data = [d for d in endata if (not d.offline and not d.sacrifice)]
    all_real_data = [d for d in endata if (d.offline and d.sacrifice)]
    half_real_data = [d for d in endata if (d.offline and not d.sacrifice)]
    
    plot_runtime([all_real_data],["SCALE-MAMBA"], "sm-time.png")
    plot_ram_usage([all_real_data],["SCALE-MAMBA"], "sm-ram.png")

    
    plot_runtime([all_fake_data, all_real_data, half_real_data], 
                ["fake offline", "real offline", "fake sacrifice"],"timing.png")
    plot_ram_usage([all_fake_data, all_real_data, half_real_data], 
                    ["fake offline", "real offline", "fake sacrifice"], "ram.png")

    for parties in range(2,9,2):
        for off,sacr in [(0,0),(1,1),(0,1)]:
            log = parse_logs.parse_logs(parties, off, sacr)
            data.append(log)

    batch_real_data = [d for d in small_batch_data if (d.offline and d.sacrifice)]
    plot_runtime([all_real_data,batch_real_data], ["batch=60K", "batch=9K"], "batch_timing.png")
    plot_ram_usage([all_real_data,batch_real_data], ["batch=60K", "batch=9K"], "batch_ram_usage.png")
    """



