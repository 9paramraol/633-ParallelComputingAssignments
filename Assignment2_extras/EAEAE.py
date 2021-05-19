A = [[1, 0, 0, 0, 0, 0, 0, 0 ],
     [1, 2, 0, 0, 0, 0, 0, 0 ],
     [1, 2, 3, 0, 0, 0, 0, 0 ],
     [1, 2, 3, 4, 0, 0, 0, 0 ],
     [1, 2, 3, 4, 5, 0, 0, 0 ],
     [1, 2, 3, 4, 5, 6, 0, 0 ],
     [1, 2, 3, 4, 5, 6, 7, 0 ],
     [1, 2, 3, 4, 5, 6, 7, 8 ]]

E = [1 , 2, 3, 4, 5, 6, 7, 8]

## --- def matrix multiplicaion with a vector
def mat_mul(A, vec):
    ans_vec = []
    for row in A:
        curr_sum = 0
        for i in range(len(vec)):
            curr_sum = add(curr_sum, mul(vec[i], row[i]))
        ans_vec.append(curr_sum)
    return ans_vec

## --- exponentiating the vector
def exp_vec(E, vec):
    ans_vec = [ exp_dict[vec[i]][E[i]] for i in range(len(E))]
    return ans_vec

def EAEAE(A, E, vec):
    ans_vec = vec
    ans_vec = exp_vec(E, ans_vec)
    ans_vec = mat_mul(A, ans_vec)
    ans_vec = exp_vec(E, ans_vec)
    ans_vec = mat_mul(A, ans_vec)
    ans_vec = exp_vec(E, ans_vec)
    return ans_vec

vec = [0, 1, 0, 0, 0, 0, 0, 0]
mat_mul(A,vec)
exp_vec(E,vec)