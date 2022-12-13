using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "New Signal")]
public class Signal : ScriptableObject
{
    public event System.Action OnRisingEdge;
    public event System.Action OnFallingEdge;

    [SerializeField] protected SignalState _startState = SignalState.None;
    protected SignalState _state = SignalState.None;

    private bool _active = false;
    private SignalState _lastState = SignalState.None;

    public SignalState state { get { return _state; } set { _state = value; } }

    private void Start()
    {
        _state = _startState;
        _lastState = _startState;
    }

    public void ToggleState()
    {
        if (_state == SignalState.Low && _lastState == SignalState.Low)
        {
            _state = SignalState.High;
            ActivateRisingEdge();
        }

        if (_state == SignalState.High && _lastState == SignalState.High)
        {
            _state = SignalState.Low;
            ActivateFallingEdge();
        }

        _lastState = _state;
    }

    public void ActivateRisingEdge()
    {
        if (OnRisingEdge != null)
            OnRisingEdge.Invoke();
    }

    public void ActivateFallingEdge()
    {
        if (OnFallingEdge != null)
            OnFallingEdge.Invoke();
    }
}