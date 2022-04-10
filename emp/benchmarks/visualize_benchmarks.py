import matplotlib.pyplot as plt
import numpy as np
import parse_logs 

# takes a list of [Bank]s and corresponding ["labels"]
def plot_runtime(data, labels,figname):
    WIDTH = .3
    ax = plt.axes()

    for idx,(dataset,lab) in enumerate(zip(data,labels)):
        pairs = [[(d.n_banks, rt) for rt in d.runtime_time] for d in dataset]
        pairs = [p for ps in pairs for p in ps]
        xs,ys = zip(*pairs)

        #xs = [d.n_banks + idx*WIDTH for d in dataset]
        #ys = [max(d.runtime_time) for d in dataset]
        plt.scatter(xs, ys, label=lab)

        plt.plot(range(2,21), np.poly1d(np.polyfit(xs, ys, 2))(range(2,21)),
            '--', label="expected runtime")

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
        pairs = [[(d.n_banks, rt) for rt in d.runtime_time] for d in dataset]
        pairs = [p for ps in pairs for p in ps]
        xs,ys = zip(*pairs)
        #xs = [d.n_banks + idx*WIDTH for d in dataset]
        #ys = [max(d.runtime_ram)/1000000 for d in dataset]
        plt.scatter(xs, ys, label = lab)

        plt.plot(range(2,21), np.poly1d(np.polyfit(xs, ys, 1))(range(2,21)),
            '--', label="expected RAM use")

        for d in dataset:
            if d.fail:
                ax.text(d.n_banks, max(d.runtime_ram)/1000000, 'X')

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

    for parties in [2,4,6]:
        log = parse_logs.parse_logs(parties, "EGJ/")
        egjdata.append(log)

        # these circuits have no input or output??? so they are very fast but don't work.
        log = parse_logs.parse_logs(parties, "EN/")
        endata.append(log)


    # plot runtime information
    #plot_runtime([egjdata, endata],["EGJ", "EN"], "emp-time.png");
    #plot_ram_usage([egjdata, endata],["EGJ", "EN"], "emp-ram.png");
    plot_runtime([egjdata],["EGJ"], "emp-time.png");
    plot_ram_usage([egjdata],["EGJ"], "emp-ram.png");

