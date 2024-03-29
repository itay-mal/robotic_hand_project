#ifndef math_helpers_cpp
#define math_helpers_cpp

void matvecmul(float m[], float in[], float out[], size_t s_in, size_t s_out) {
    /*
    out = m * in matrix vector multiplication
    */
    for (size_t i=0; i<s_out; i++) {
        out[i] = 0;
        for (size_t j=0; j<s_in; j++) {
            out[i] += m[i*s_in+j]*in[j];
        }
    }
}

void matvecmulacc(float m[], float in[], float out[], size_t s_in, size_t s_out) {
    /*
    out = m * in matrix-vector multiplication with accumulation
    */
    for (size_t i=0; i<s_out; i++) {
        for (size_t j=0; j<s_in; j++) {
        out[i] += m[i*s_in+j]*in[j];
        }
    }
}

void vecvecadd(float a[], float b[], float out[], size_t s) {
    /*
    out = a + b vector add
    */
    for (size_t i=0; i<s; i++) {
        out[i] = a[i]+b[i];
    }
}

void vecvecaddinplace(float a[], float out[], size_t s) {
    /*
    out = out + a vector add
    */
    for (size_t i=0; i<s; i++) {
        out[i] += a[i];
    }
}

void veccopy(float a[], float out[], size_t s) {
    /*
    out = a vector copy
    */
    for (size_t i=0; i<s; i++) {
        out[i] = a[i];
    }
}

#define RELU_MAX 30
void relu(float a[], float out[], size_t s) {
    /*
    apply trimmed relu activation function (vector operation):
    out = x if 0 < x < RELU_MAX else 0
    */
    for (size_t i=0; i<s; i++) {
        if (a[i]>0) {
            out[i] = a[i];
            if (a[i]>RELU_MAX) {out[i]=RELU_MAX;}
        }
        else {out[i] = 0;}
    }
}

void reluinplace(float a[], size_t s) {
    /*
    apply trimmed relu activation function (vector operation) in place:
    a = a if 0 < a < RELU_MAX else 0
    */
    for (size_t i=0; i<s; i++) {
        if (a[i]>0) {
        if (a[i]>RELU_MAX) {a[i]=RELU_MAX;}
        }
        else {a[i] = 0;}
    }
}

void softmaxinplace(float a[], size_t s) {
    /*
    apply softmax in place:
    a[i] = a[i]/sum(a[0:s])
    */
    float sum = 0;
    for (size_t i=0; i<s; i++) {
        a[i] = std::exp(a[i]);
        sum += a[i];
    }
    for (size_t i=0; i<s; i++) {
        a[i] /= sum;
    }
}

void tanhinplace(float a[], size_t s) {
    /*
    apply hyperbolic tangent in place (vector):
    a = tanh(a)
    */
   for (size_t i=0; i<s; i++) {
        a[i] = std::tanh(a[i]);
    }
}

void sigmoidinplace(float a[], size_t s) {
    /*
    apply sigmoid in place (vector):
    a = 1/ (1 + e^(-a))
    */
   for (size_t i=0; i<s; i++) {
        a[i] = 1 / (1 + std::exp(-a[i]));
    }
}
#endif