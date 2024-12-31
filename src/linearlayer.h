#pragma once
#include <fstream>
#include <iostream>
#include <vector>

// TODO: make input, output templated?
template <typename T = float> struct LinearLayer
{
    int num_inputs;
    int num_outputs;
    T **weights = nullptr;
    T *bias = nullptr;

    constexpr LinearLayer() = default;

    constexpr LinearLayer(int num_inputs, int num_outputs) : num_inputs(num_inputs), num_outputs(num_outputs)
    {
        weights = new T *[num_inputs];
        for (int i = 0; i < num_inputs; ++i)
        {
            weights[i] = new T[num_outputs];
        }
        bias = new T[num_outputs];
    }
    constexpr LinearLayer(const LinearLayer<T> &other) : num_inputs(other.num_inputs), num_outputs(other.num_outputs)
    {
        weights = new T *[num_inputs];
        for (int i = 0; i < num_inputs; ++i)
        {
            weights[i] = new T[num_outputs];
            std::copy(other.weights[i], other.weights[i] + num_outputs, weights[i]);
        }
        bias = new T[num_outputs];
        std::copy(other.bias, other.bias + num_outputs, bias);
    }

    LinearLayer &operator=(LinearLayer &&other) noexcept
    {
        this->~LinearLayer();

        num_inputs = other.num_inputs;
        num_outputs = other.num_outputs;
        weights = new T *[num_inputs];
        for (int i = 0; i < num_inputs; ++i)
        {
            weights[i] = new T[num_outputs];
            std::copy(other.weights[i], other.weights[i] + num_outputs, weights[i]);
        }

        bias = new T[num_outputs];
        std::copy(other.bias, other.bias + num_outputs, bias);

        return *this;
    }

    virtual T *linear(T *output, const T *input)
    {
        for (int i = 0; i < num_outputs; ++i)
            output[i] = bias[i];
        for (int i = 0; i < num_inputs; ++i)
        {
            for (int j = 0; j < num_outputs; ++j)
            {
                output[j] += input[i] * weights[i][j];
            }
        }

        return output + num_outputs;
    }

    constexpr int get_input_size() const
    {
        return num_inputs;
    }
    constexpr int get_output_size() const
    {
        return num_outputs;
    }

    bool write(std::ofstream &file) const
    {
        try
        {
            file.write(reinterpret_cast<const char *>(&num_inputs), sizeof(num_inputs));
            file.write(reinterpret_cast<const char *>(&num_outputs), sizeof(num_outputs));
            for (int i = 0; i < num_inputs; ++i)
            {
                file.write(reinterpret_cast<const char *>(weights[i]), num_outputs * sizeof(T));
            }
            file.write(reinterpret_cast<const char *>(bias), num_outputs * sizeof(T));
        }
        catch (...)
        {
            std::cerr << "Error while saving weights!\n";
            return false;
        }
        return true;
    }

    bool read(std::ifstream &file)
    {
        int stored_inputs, stored_outputs;
        file.read(reinterpret_cast<char *>(&stored_inputs), sizeof(stored_inputs));
        file.read(reinterpret_cast<char *>(&stored_outputs), sizeof(stored_outputs));

        if (stored_inputs != num_inputs || stored_outputs != num_outputs)
        {
            return false;
        }

        try
        {
            for (int i = 0; i < num_inputs; ++i)
            {
                file.read(reinterpret_cast<char *>(weights[i]), num_outputs * sizeof(T));
            }

            file.read(reinterpret_cast<char *>(bias), num_outputs * sizeof(T));
        }
        catch (...)
        {
            std::cerr << "Error while reading weights!\n";
            return false;
        }
        return true;
    }
    virtual ~LinearLayer()
    {
        if (weights)
        {
            for (int i = 0; i < num_inputs; ++i)
                delete[] weights[i];
        }
        delete[] weights;
        delete[] bias;
        weights = nullptr;
        bias = nullptr;
    }
    void cerr_print(std::ofstream &s) const
    {
        s << "LAYER: " << num_inputs << ", " << num_outputs << '\n';
        s << "weights:\n";
        for (int i = 0; i < num_inputs; ++i)
        {
            s << i << ":";
            for (int j = 0; j < num_outputs; ++j)
                s << ' ' << weights[i][j];
            s << '\n';
        }
        s << "bias:";
        for (int j = 0; j < num_outputs; ++j)
            s << ' ' << bias[j];
        s << '\n';
        s << "--------------------------\n\n";
    }
};

template <typename T = float> struct TrainLayer : LinearLayer<T>
{
    T *raw = nullptr;       // before activation
    T *activated = nullptr; // after activation

    TrainLayer() = default;

    TrainLayer(int num_inputs, int num_outputs) : LinearLayer<T>(num_inputs, num_outputs)
    {
        raw = new T[num_outputs];
        activated = new T[num_outputs];
    }
    TrainLayer(const TrainLayer<T> &other) : LinearLayer<T>(other)
    {
        raw = new T[other.num_outputs];
        std::copy(other.raw, other.raw + other.num_outputs, raw);
        activated = new T[other.num_outputs];
        std::copy(other.activated, other.activated + other.num_outputs, activated);
    }

    TrainLayer &operator=(TrainLayer &&other) noexcept
    {
        this->~TrainLayer();

        LinearLayer<T>::operator=(std::move(other));
        raw = new T[other.num_outputs];
        std::copy(other.raw, other.raw + other.num_outputs, raw);
        activated = new T[other.num_outputs];
        std::copy(other.activated, other.activated + other.num_outputs, activated);
        return *this;
    }

    T *linear(T *output, const T *input) override
    {
        for (int i = 0; i < LinearLayer<T>::num_outputs; ++i)
            raw[i] = LinearLayer<T>::bias[i];
        for (int i = 0; i < LinearLayer<T>::num_inputs; ++i)
        {
            for (int j = 0; j < LinearLayer<T>::num_outputs; ++j)
            {
                raw[j] += input[i] * LinearLayer<T>::weights[i][j];
            }
        }
        std::copy(raw, raw + LinearLayer<T>::num_outputs, output);

        return output + LinearLayer<T>::num_outputs;
    }

    void activate(T func(T))
    {
        for (int i = 0; i < LinearLayer<T>::num_outputs; ++i)
            activated[i] = func(raw[i]);
    }

    ~TrainLayer() override
    {
        delete[] raw;
        delete[] activated;
        raw = nullptr;
        activated = nullptr;
    }
};
// typedef LinearLayer<> LinearLayer;