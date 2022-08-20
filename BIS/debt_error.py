from __future__ import print_function
import numpy as np
import csv

def defaulters( L, e, p ):
	n = L.shape[0]
	Pi = L/L.sum(axis=0)
	p_bar = L.sum(axis=0).reshape(n)
	D = np.zeros( shape=n, dtype=int )
	for i in range(n):
		if e[i] + sum( [p[j]*Pi[j][i] for j in range(n)] ) < p_bar[i]:
			D[i] = 1
	return D

def new_p(D,L,e,p,approximate=False,k=10):
	n = L.shape[0]
	Pi = L/L.sum(axis=0)
	p_bar = L.sum(axis=0).reshape(n)
	Lambda = np.diag(D)
	Id = np.identity(n,dtype=np.float64)
	A =	Lambda.dot(Pi.transpose()).dot(Lambda)
	b = Lambda.dot(Pi.transpose()).dot(Id-Lambda).dot(p_bar) + Lambda.dot(e) + (Id-Lambda).dot(p_bar)
	if approximate == False:
		new_p = np.linalg.inv(Id-A).dot(b)
	if approximate == True:
		new_p = p_bar
		for _ in range(k):
			new_p = A.dot(new_p) + b 
	return new_p


def clearing(L,e,approximate=False,k=10):
	n = L.shape[0]
	p_bar = L.sum(axis=0).reshape(n)
	Pi = L/L.sum(axis=0)
	p = p_bar
	for i in range(n):
		D = defaulters(L,e,p)		
		p = new_p(D,L,e,p,approximate,k=k)
	
	
	if not approximate:
		eps=.001
		assert all( [p[i] <= p_bar[i]+eps for i in range(len(p))] )
		assert all( [0 <= p[i] + eps for i in range(len(p))] )
		v = e + Pi.transpose().dot(p) - p
		assert all( [abs(v[i]) < eps or abs(p[i]-p_bar[i]) < eps for i in range(len(p))] )
	return p

def gen_plot_data(n,min_k,max_k):
	t = 200
	data = []
	for k in range(min_k,max_k):
		relative_errors = []
		for _ in range(t):
			L = np.random.uniform(size=(n,n))
			e = np.random.uniform(size=n)
			p_true = clearing( L, e, approximate=False )
			p_approx = clearing( L, e, approximate=True, k=k )
			relative_error = np.linalg.norm(p_true-p_approx,ord=1)/np.linalg.norm(p_true,ord=1)
			relative_errors.append(relative_error)
		data.append([k,np.mean(relative_errors),np.quantile(relative_errors,.025),np.quantile(relative_errors,.975)])
	return data


if __name__=='__main__':
	d = gen_plot_data(20,1,20)
	with open('debt_error.csv', 'w', newline='') as csvfile:
		datawriter = csv.writer(csvfile, delimiter=',', quotechar="'", quoting=csv.QUOTE_MINIMAL)
		for r in d:
			datawriter.writerow(r)

