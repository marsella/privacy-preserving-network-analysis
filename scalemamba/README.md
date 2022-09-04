## SCALE-MAMBA

## Docker Setup
Create a Docker image. This will take a few minutes. You only have to do this
once.
```
$ docker build -t scalemamba .
```
Spin up a Docker container from the image. 
```
$ docker run -it --rm scalemamba 
```

## Architecture
SCALE-MAMBA executes a two-phase GMW-based protocol. 
The phases run back-to-back with a single executable.
Users need to configure the parameters for the computation, compile the program
with those parameters, and then run each computation party.

## Running basic benchmarks

The container builds the SCALE-MAMBA engine and installs our examples. The computations themselves are in `source/egj.mpc` and `source/noe.mpc`; when the container is built, they are installed in correspondingly-named directories in the `Programs` directory.

We defined custom I/O to read data from files. This is contained in `Input_Output_File.h` and `Input_Output_File.cpp`. We have a small patch to the SCALE-MAMBA engine to explicitly use the I/O defined in `sm.patch`; this is installed during the Docker build.

The following instructions allow you to run the software manually. In practice, we used `benchmark.sh` to automate all these steps: setup and compilation, running all players, and writing results to file (currently, `/root/benchmarks` in the container). It's currently set to run with 4 players (takes <1min). Feel free to take this shortcut by running:

```
$ ./benchmark.sh 1 1
```

The parameters for `benchmark.sh` determine whether to use real (secure) data for the preprocessing and sacrifice phases. Briefly, the players need to have some coordinated data generated "offline" -- before they can run a computation. They can either have a trusted third party create it for them ("fake"), or generate it themselves ("real"). Generating it themselves is quite slow and takes a lot of resources; for testing this experiment, you might prefer to use "fake" data for efficiency, but we ran experiments with both real and fake offline data generation.
As an example, running 6 players takes about 2 minutes with fake offline and sacrifice data, and 25 minutes (and a lot of RAM) with real data.

You can change the program that is benchmarked by modifying line 10 (the options are `noe` or `egj`). You can change the number of players by modifying line 35. See Fig 4 for estimated time and RAM usage for more players.

## Manually running the examples
If you'd like to see the whole process, you can try running the examples manually.

First step is to set up parameters for the computation, including network
participants and secret-sharing setup. The framework provides a setup script to
interactively produce the appropriate config files.

Run the following commands. You will need to create BOTH config files (Secret
Sharing and Cert). 
```
$ cd ~/SCALE-MAMBA
$ ./Setup.x
What do you want to set up?
    1) Certs
    2) Secret Sharing
    3) Both
Enter a number (1-3)..
```
Enter `3`.

We've placed all necessary certificate infrastructure in `SCALE-MAMBA/Cert-Store`. We use the default/recommended naming for the root certificate (`RootCA`) and the individual player certificates (`Player<i>.crt`). You can start with 2 players. We ran examples using localhost (127.0.0.1) for the IP address. You can test with real or fake offline and sacrifice phases (real ones will take longer & more resources).

For secret sharing setup, we used Full Threshold sharing (option 1), with
varying length modulus. Our programs require a minimum of 123. _IMPORTANT_: Full
threshold sharing insecurely generates keys! For secure key generation, use a
different sharing scheme.

This setup script produces two configuration files: `Data/NetworkingData.txt` and
`Data/SharingData.txt`.

Then, compile the directory containing the program you wish to run, either `noe` or `egj`.
```
$ ./compile.py Programs/noe
```
Generate synthetic data (written to `Data/`).
```
$ python geninput.py <#parties>
```

Then, run each computation party. 
```
$ ./Player.x <id> Programs/<ex>
```

To run our examples, we ran all players together locally:
```
$ ./Player.x 0 Programs/noe & ./Player.x 1 Programs/noe
```


## Visualizing data for figures
To generate the figures in our text, we first ran `benchmark.sh` for various numbers of parties. This does require a fairly robust computer with a significant amount of RAM. All our raw benchmarking results are in `benchmarks/` and log the output of the SCALE-MAMBA executable and the `time` command, which measures overall resource usage of the executable. There are 4 main experiments:
- `results`: Results of running `n`-party computations for EN and EGJ where every party is both a data owner and a participant. To replicate, run `benchmark.sh 1 1` without modification.
- `batch_logs`: Results of changing the batch size for offline data computation. A given program requires a fixed amount of offline data. The data is generated in batches, so the number of rounds necessary to compute all the data will depend on the batch size. Here, we varied the batch size to compare offline data generation time to the default in `results`. To replicate, modify line 16 in `src/config.h` with the desired batch size (e.g. `#define sz_offline_batch 9000`). The default in `results` is 60000, and these were generated with 9000.
- `ram_logs`: These measure `n`-party computations for EN with real and fake offline data (with the goal of measuring the RAM requirements of the two offline phases). To replicate, run `benchmark.sh <preprocessing> <sacrifice>` with different settings for offline data: `benchmark.sh 1 1` for both fake, `0 1` for real preprocessing and fake sacrifice, and `0 0` for both real.
- `fixedparty-results`: Results of running the computation with varying computation parties but only one data owners (e.g. Player 0 provides input, but all others don't). To replicate, modify `geninput.py` to set `ONEPARTYINPUT = True`.

The file `visualize_benchmarks.py` parses these data to generate the figures used in the text. You can run it from your machine (e.g. not in the docker container).
```
$ python3 visualize_benchmarks.py
```
- To make Table ??, it parses the results and fixed-party results to measure the maximum time and maximum RAM consumed by any party in each computation for each algorithm. These results are written to CSVs in the `csv` directory and formatted in LaTeX to make the table.

- To make Fig ?? and ??, it takes the same maximum time and RAM data for the standard `results` and plots it to `sm_ram.png` and `sm_time.png`.

- To make Fig ?? and ??, it measures time and RAM data for the varying `ram_logs` and plots it to `timing.png` and `ram.png`.

- To make Fig ?? and ??, it measures time and RAM data for the `batch_logs` and plots it with the corresponding data from `results` in `batch_timing.png` and `batch_ram_usage.png`.
