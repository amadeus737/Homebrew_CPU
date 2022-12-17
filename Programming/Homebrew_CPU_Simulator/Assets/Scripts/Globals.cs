using UnityEngine;

public enum SignalActivation { None, RisingEdge, FallingEdge, ActiveLow, ActiveHigh }
public enum SignalState { None, High, Low }
public enum RegisterLabel { A, B, C, D }
public enum ValueState { Error, TriState, Good }
public enum ObjectState { TriState, Error, OK }
public enum ClockMode { None, Step, Run }

[System.Serializable]
public class Value
{
    [SerializeField] private ValueState _state;
    [SerializeField] private ulong _value;
    [SerializeField] private ulong _maxVal;

    public ValueState state { get { return _state; } set { _state = value; }}
    public ulong value { get { return _value; } set { _value = value; }}
    public ulong maxVal { get { return _maxVal; } set { _maxVal = value; }}
}

[System.Serializable]
public class ControlledBus
{
    public ControlLine controlLine;
    public Bus bus;
}