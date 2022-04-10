
class TestData:
    def __init__(self):
        self.n_banks = 0
        self.compile_ram = 0
        self.compile_time = 0.0

        self.runtime_ram = 0
        self.runtime_time = 0
        self.runtime_completed = 0

        self.fail = False
        self.correct = None

    def print_verbose(self):
        print("%d party computation"%(self.n_banks))
        print("\t%d kybtes ram to compile in %d seconds"%(self.compile_ram, self.compile_time))
        print("\taverage runtime was %d seconds with %d kbytes ram"%(sum(self.runtime_time)/len(self.runtime_time),sum(self.runtime_ram)/len(self.runtime_ram)))
        print("\tmac fail? %s"%self.mac_fail)
        print("\t%d parties completed computation"%self.runtime_completed)

def parse_time(time_line):
    stime = 0
    time = time_line.split()[-1].split(':')
    if len(time) > 2:
        stime = 3600 * int(time[0])
    stime += float(time[-1]) + 60 * int(time[-2])
    return stime

# n_banks: int
def parse_logs(n_banks, alg="iterative", HOME = "./results/"):
    log = TestData()
    log.n_banks = n_banks

    runtime_file = HOME + "output{}.{}.dat".format(n_banks,alg)
    with open(runtime_file, 'r') as f:
        runtime_lines = f.readlines()

    correct = True
    for line in runtime_lines:
        if "Elapsed (wall clock) time" in line:
            log.runtime_time = parse_time(line)

        if "Maximum resident set size" in line:
            log.runtime_ram = int(line.split()[-1])

        if "ommand exited" in line or "ommand terminated" in line:
            log.fail = True

        # check correctness for BIS data: the default list should always be boolean
        # only applies to [inversion, iterative]
        if "Round " in line:
            valarr = [float(x) for x in line.split(':')[1][2:-2].split(',')]
            invalid = list(filter(lambda x: not(x == 1. or x == 0.), valarr))
            correct = correct and len(invalid) == 0

        # check correctness for simulated EGJ data
        # expected results are small (0-3). bad results are enormous
        if "result" in line:
            valarr = [float(x) for x in line.split(':')[1][2:-2].split(',')]
            invalid = list(filter(lambda x: x > 100, valarr)) 
            correct = correct and len(invalid) == 0

    log.correct = correct
    #print(runtime_file, correct)
    return log

if __name__ == '__main__':
    for parties in [2,4]:
        log = parse_logs(parties)
        log.print_verbose()

    # TODO:
    # printed result?


