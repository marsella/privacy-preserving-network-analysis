# BIS data

## Dependencies
Install a recent verison of python3. This was tested with Python 3.8.10.

Optionally, create and enter a new virtual environment:
```
$ python3 -m venv env
$ source env/bin/activate
```

Install packages:
```
$ pip install networkx matplotlib
```

## Retrieve the BIS data
We include a limited set of the BIS data here. 

Unfortunately, since the time we wrote this scraper in 2018, BIS has changed its URL scheme and query engine, so these files don't work right now:
- `get_data.sh` retrieves the full consolidated banking statistics dataset from the BIS website and unzips it.
- `format.R` parses it to retrieve the relevant values for our work. See the comments in that file and Section ?? for details.

## Format data for figures

### Plot the BIS data 
The `analyze.py` file will generate a plot showing the debt graph among the top `n` reporting countries from BIS.
Each node represents a country, with node size corresponding to total volume of assets.
Each edge represents the debt owed by the source country to the destination country, with the thickness of the edge corresponding to debt size.

The output is written to `bis_nolabel.png`; as noted in the text, the country labels get cut off by this plotting software. Instead, we write them to `BISnames.csv` and manually re-add them in the LaTeX code to get Figure ??.

The `-n` argument determines the number of nodes, up to 21 countries, and the software will report the number of nodes and edges pulled from the BIS data.

```
$ python3 analyze.py -n 10
10 reporters, 78 total edges
```

### Format the BIS data for a table
The `make_csv.py` formats the BIS data into a CSV to fill Table ??.

It parses data scraped directly from the BIS website, compute exact and approximate (TODO: clearing?) and formats the result, along with the corresponding country and their equity reserves, in `BIS_data.csv`.

### Compute synthetic debt error 
The `debt_error.py` file generates synthetic data for a group of 20 entities, computes the exact and approximate (TODO: clearing?) and formats the result in `debt_error.csv`. This is used to create Figure ??.


## Format the BIS data for MPC computations
`analyze.py` also handles formatting the data for the various software libraries.
Each MPC library and the insecure implementation requires input in a slightly different format.
Setting the `--outputtype <LIB>` option will produce data formatted appropriately for the working demos (MPyC, SCALE-MAMBA, and the insecure implementation) and move it to the relevant directory.
See the helptext for how to specify each library.
```
$ python3 analyze.py --help
```

### SCALE-MAMBA
I think that the SCALE-MAMBA container defined in the main directory makes some slightly dfiferent assumptions about synthetic data that can't be applied to the real data, so there's a separate container to run SCALE-MAMBA here. 
This version differs in the following ways:
- it only runs the Noe implementation, and doesn't include the EGJ implementation or the generate-synthetic-input file at all
- in the NOE implementation, it doesn't call `initializeSharedData` and it hardcodes 21 banks
- it installs some python stuff
- it includes and runs `testondata.sh`, which sets up and runs the computation on the BIS data generated as above.

Anyway, when we format BIS data for SCALE-MAMBA, it puts it here.