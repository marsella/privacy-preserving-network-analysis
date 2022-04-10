
class TestData:
    def __init__(self):
        self.n_banks = 0
        self.rounds = 0
        self.compile_ram = 0
        self.compile_time = 0.0

        self.runtime_ram = []
        self.runtime_time = []
        self.runtime_completed = 0

        self.fail = False

    def print_verbose(self):
        print("%d party computation"%(self.n_banks))
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
def parse_logs(n_banks, HOME = "./results/"):
    #compile_file = HOME + "output%d.p%d.dat"%(n_banks)

    log = TestData()
    log.n_banks = n_banks

    '''
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
    '''
    for player in range(n_banks):
        runtime_file = HOME + "output%d.p%d.dat"%(n_banks,player+1)
        with open(runtime_file, 'r') as f:
            runtime_lines = f.readlines()

        for line in runtime_lines:
            if "Elapsed (wall clock) time" in line:
                log.runtime_time.append(parse_time(line))

            if "Maximum resident set size" in line:
                log.runtime_ram.append(int(line.split()[-1]))

            if "ommand exited" in line or "ommand terminated" in line:
                log.fail = True

    return log


if __name__ == '__main__':
    for parties in [2,4]:
        log = parse_logs(parties)
        log.print_verbose()

    # TODO:
    # printed result?


