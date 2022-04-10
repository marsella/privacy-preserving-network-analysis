
class TestData:
    def __init__(self):
        self.n_banks = 0
        self.rounds = 0
        self.compile_ram = 0
        self.compile_time = 0.0

        self.runtime_ram = []
        self.runtime_time = []
        self.runtime_completed = 0

        self.mac_fail = False
        self.offline = False
        self.sacrifice = False

    def print_verbose(self):
        print("%d party computation with %s offline and %s sacrifice data"%(self.n_banks,self.offline, self.sacrifice))
        print("\t%d rounds"%self.rounds)
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
# off, sacr: int (0 if real data, 1 if fake data)
def parse_logs(n_banks, off, sacr, HOME = "./ram_logs/"):
    compile_file = HOME + "output%d.%d.%d.dat"%(n_banks,off,sacr)

    log = TestData()
    log.n_banks = n_banks
    log.offline = (off == 0)
    log.sacrifice = (sacr == 0)

    log = parse_compiler(compile_file, log)

    for player in range(n_banks):
        runtime_file = HOME + "output%d.%d.%d.p%d.runtime.dat"%(n_banks,off,sacr,player)
        log = parse_runtime(runtime_file, log)

    return log


def parse_egj_logs(n_banks, HOME="./results/"):
    compile_file = HOME + "output%d.egj.dat"%(n_banks)

    log = TestData()
    log.n_banks = n_banks
    log.offline = 0
    log.sacrifice = 0

    log = parse_compiler(compile_file, log)
    for player in range(n_banks):
        runtime_file = HOME + "output%d.egj.p%d.runtime.dat"%(n_banks,player)
        log = parse_runtime(runtime_file, log)

        #TDOD: add correctness checks

    return log

def parse_fixed_logs(n_banks, HOME="./fixedparty-results/"):
    compile_file = HOME + "fixedparty%d.compile.noe.dat"%(n_banks)

    log = TestData()
    log.n_banks = n_banks
    log.offline = 0
    log.sacrifice = 0

    log = parse_compiler(compile_file, log)
    for player in range(3):
        runtime_file = HOME + "fixedparty%d.noe.p%d.runtime.dat"%(n_banks,player)
        log = parse_runtime(runtime_file, log)

    return log
    

def parse_compiler(compile_file, log):
    with open(compile_file, 'r') as f:
        compile_lines = f.readlines()
    
    for line in compile_lines:
        if "rounds of communication" in line:
            log.rounds = int(line.split()[2])

        # time is either h:mm:ss or m:ss
        if "Elapsed (wall clock) time" in line:
            log.compile_time = parse_time(line) 
            
        # ram should be in kbytes 
        if "Maximum resident set size" in line:
            log.compile_ram = int(line.split()[-1])
    return log

def parse_runtime(runtime_file, log):
    with open(runtime_file, 'r') as f:
        runtime_lines = f.readlines()

    for line in runtime_lines:
        if "Elapsed (wall clock) time" in line:
            log.runtime_time.append(parse_time(line))

        if "Maximum resident set size" in line:
            log.runtime_ram.append(int(line.split()[-1]))

        if "mac_fail" in line:
            log.mac_fail = True

        if "End of prog" in line:
            log.runtime_completed += 1

    return log


if __name__ == '__main__':
    for parties in range(2,9,2):
        for off,sacr in [(0,0),(1,1)]:
            log = parse_logs(parties, off, sacr)
            log.print_verbose()

    # TODO:
    # printed result?


