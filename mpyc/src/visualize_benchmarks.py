import matplotlib.pyplot as plt
import numpy as np
import parse_logs 

def plot_errors():
    mean_scl = []
    mean_sec = []
    mean_err = []

    parties = range(5,10)
    for n in parties:
        fname = "results/{}.dat".format(n)

        # sort data into tuples of lists
        # 1 tuple = 1 trial: (clr, scl, sec)
        data = [line.split(' ') for line in open(fname,'r').readlines()] 
        data = [[float(x) for x in line[1:]] for line in data]
        data = list(zip(*[data[i::3] for i in range(3)]))

        # compute errors in scaled data
        n = len(data[0][0]) * len(data)
        sclerr = np.matrix([[abs(1 - s/c) for (s,c) in zip(scl,clr)] for (clr, scl, sec) in data])
        sclerr = np.reshape(sclerr, n)
        print("clear vs scale", np.mean(sclerr))
        mean_scl += [np.mean(sclerr)]

        secerr = np.matrix([[abs(1 - s/c) for (s,c) in zip(sec,clr)] for (clr, scl, sec) in data])
        secerr = np.reshape(secerr, n)
        print("clear v secure",np.mean(secerr))
        mean_sec += [np.mean(secerr)]

        err = np.matrix([[abs(1 - s/c) for (s,c) in zip(sec,scl)] for (clr, scl, sec) in data])
        err = np.reshape(err, n)
        print("scale v secure", np.mean(err))
        mean_err += [np.mean(err)]

    print(mean_scl)
    plt.scatter(parties, mean_scl, label="clear vs scale")
    plt.scatter(parties[:-1], mean_sec[:-1], label="clear vs secure")
    plt.scatter(parties[:-1], mean_err[:-1], label="scale vs secure")

    plt.xlabel("number of participants")
    plt.xticks(parties)
    plt.ylabel("error (avg(1-(a/b)))")
    plt.legend()

    plt.savefig("error.png")
    plt.clf()

# writes data to csvs to use in gnuplot
def make_csvs(itrdata, invdata, egjdata):
    with open("csv/mpyc.runtime.csv", 'w') as f:
        f.write("n, iterative, inversion, egj\n")
        for (itr, inv, egj) in zip(itrdata, invdata, egjdata):
            assert itr.n_banks == inv.n_banks
            assert itr.n_banks == egj.n_banks
            f.write("{},{},{},{}\n".format(itr.n_banks, itr.runtime_time, inv.runtime_time, egj.runtime_time))

    ramscale = 1000000 
    with open("csv/mpyc.ram.csv", 'w') as f:
        f.write("n, iterative, inversion, egj\n")
        for (itr, inv,egj) in zip(itrdata, invdata, egjdata):
            assert itr.n_banks == inv.n_banks
            assert itr.n_banks == egj.n_banks
            f.write("{},{},{},{}\n".format(itr.n_banks, itr.runtime_ram/ramscale, inv.runtime_ram/ramscale, egj.runtime_ram/ramscale))

# takes a list of [Bank]s and corresponding ["labels"]
def plot_runtime(data, labels,figname):
    ax = plt.axes()

    for idx,(dataset,lab) in enumerate(zip(data,labels)):
        dataset = [d for d in dataset if d.correct] #filter out bad values
        xs = [d.n_banks for d in dataset]
        ys = [d.runtime_time for d in dataset]
        plt.scatter(xs, ys, label=lab)

        plt.plot(range(2,21), np.poly1d(np.polyfit(xs, ys, 2))(range(2,21)),
            '--', label="expected runtime")
        
        try:
            badx, bady = zip(*[(d.n_banks, d.runtime_time) for d in dataset if not d.correct])
            plt.scatter(badx, bady, marker='x', color='black',label="incorrect results")
        except:
            print("no faults for", lab)

    # add time context
    #ax.hlines(y=60, xmin=2, xmax=10,linestyle='dashed',color='gray')
    #ax.text(16,70,'1 minute',color='gray')
    #ax.hlines(y=3600, xmin=2, xmax=20,linestyle='dashed',color='gray')
    #ax.text(16,3900,'1 hour',color='gray')

    # add plot info
    #plt.yscale('log')
    plt.xticks(np.arange(2,22,2))
    plt.xlabel("number of participants")
    plt.ylabel("time (seconds)")
    plt.legend(framealpha=1)

    plt.savefig(figname)
    plt.clf()



# plot runtime ram usage
def plot_ram_usage(data, labels,figname):
    ax = plt.axes()
    scale = 1000000 
    
    for idx,(dataset,lab) in enumerate(zip(data,labels)):
        dataset = [d for d in dataset if d.correct] #filter out bad values
        xs = [d.n_banks for d in dataset]
        ys = [d.runtime_ram / scale for d in dataset]
        plt.scatter(xs, ys, label = lab)

        plt.plot(range(2,21), np.poly1d(np.polyfit(xs, ys, 1))(range(2,21)),
            '--', label="expected RAM use")

        try:
            badx, bady = zip(*[(d.n_banks, d.runtime_ram / scale) for d in dataset if not d.correct])
            plt.scatter(badx, bady, marker='x', color='black',label="incorrect results")
        except:
            print("no faults for", lab)

    plt.xticks(np.arange(2,10,2))
    plt.xlabel("number of participants")
    plt.ylabel("peak RAM usage (gB)")
    plt.legend(loc="upper left")

    plt.savefig(figname)
    plt.clf()

def plot_correctness(data, labels, figname):
    for (dataset, lab) in zip(data, labels): 
        good = [d for d in dataset if d.correct]
        bad = [d for d in dataset if not d.correct]

        print("good:", [d.n_banks for d in good])
        print("bad:", [d.n_banks for d in bad])

        plt.scatter([d.n_banks for d in good],
                    [0] * len(good),
                    marker='o');
        
        plt.scatter([d.n_banks for d in bad],
                    [1] * len(bad),
                    marker='x');

    plt.xlabel("number of participants")
    plt.ylabel("alpha")
    plt.savefig(figname)
    plt.clf()


if __name__ == "__main__":
    itrdata = []
    invdata = []
    egjdata = []

    #plot_errors()

    for parties in range(2,9):
        log = parse_logs.parse_logs(parties)
        itrdata.append(log)

        log = parse_logs.parse_logs(parties, alg="inversion")
        invdata.append(log)

        log = parse_logs.parse_logs(parties, alg="EGJ")
        egjdata.append(log)
        
    make_csvs(itrdata, invdata, egjdata)

    # plot runtime information
    ds = [itrdata, invdata, egjdata]
    ls = ["[EN01]", "[EN01] + [BBSdV19]", "[EGJ14]"]
    plot_runtime(ds,ls, "mpyc-time.png");
    plot_ram_usage(ds,ls, "mpyc-ram.png");

    #plot_correctness([invdata],["inversion"], "mpyc-correct.png");
 
