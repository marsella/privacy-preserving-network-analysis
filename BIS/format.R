df = read.csv("WS_CBS_PUB_csv_col.csv")
years = seq(1984,2018) 
qcols = paste0( rep( paste0( "X", years ), each=4  ), rep( paste0(".Q", seq(1,4)) , length(years) ) )
qcols = intersect( qcols, colnames(df) )

period = "X2018.Q2"

# initial parsing: remove aggregate rows, restrict to domestic banks
df = df[which(df$Reporting.country != "All countries" ),]
df = df[which(df$Reporting.country != "All reporting countries" ),]
df = df[which(df$CBS_BANK_TYPE == "4B"),] # domestic banks

# Format liabilities (country-to-country edges)
liab = df
liab = liab[which(liab$Counterparty.country != "All countries excluding residents" ),]
liab = liab[which(liab$Counterparty.country != "All countries" ),]
liab = liab[which(liab$Balance.sheet.position == "International claims"),]
liab = liab[which(liab$CBS.bank.type == "Domestic banks"),]

liab = liab[,c("Reporting.country","Counterparty.country",period)]

# takes the sum of period data. 
# representing international claims reporter -$--> counterparty
frm = as.formula( paste0( period, "~ Reporting.country + Counterparty.country" ) )
A = aggregate( frm, data=liab, FUN=sum )

# Print liabilities
outfile=paste0(period,".csv")
print( paste0( "Writing ", nrow(liab), " rows to ", outfile ) )
write.csv( A, file=outfile )

# Format equity (per-country reserves)
eq = df
eq = eq[which(eq$L_POSITION == "K"),] # capital / equity

eq = eq[,c("Reporting.country","CBS.bank.type","Balance.sheet.position",period)]

frm = as.formula( paste0( period, "~ Reporting.country" ) )
A = aggregate( frm, data=eq, FUN=sum )

# Print equity
outfile=paste0(period,".equity.csv")
print( paste0( "Writing ", nrow(eq), " rows to ", outfile ) )

write.csv( A, file=outfile )


