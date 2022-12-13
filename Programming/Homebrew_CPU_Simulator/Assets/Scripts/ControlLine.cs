using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Control Line")]
public class ControlLine : Signal
{
    [SerializeField] private SignalActivation _activation_type = SignalActivation.None;
    
    private float _wait_time = 0.0f;
    private IEnumerator _coroutine = null;

    public bool active = false;

    private void OnEnable()
    {
        if (_activation_type == SignalActivation.RisingEdge) OnRisingEdge += Activate;
        if (_activation_type == SignalActivation.FallingEdge) OnFallingEdge += Activate;
    }

    private void OnDisable()
    {
        if (_activation_type == SignalActivation.RisingEdge) OnRisingEdge -= Activate;
        if (_activation_type == SignalActivation.FallingEdge) OnFallingEdge -= Activate;
    }

    private void Start()
    {
        _wait_time = Clock.Instance().edge_ratio * Clock.Instance().halfPeriod;
    }

    private void Update()
    {
        if (_activation_type == SignalActivation.ActiveHigh && state == SignalState.High ||
            _activation_type == SignalActivation.ActiveLow && state == SignalState.Low)
        {
            Activate();
        }

        if (_activation_type == SignalActivation.ActiveHigh && state == SignalState.Low ||
            _activation_type == SignalActivation.ActiveLow && state == SignalState.High)
        {
            Deactivate();
        }

        if ((_activation_type == SignalActivation.RisingEdge || _activation_type == SignalActivation.FallingEdge) && active && _coroutine == null)
        {
            DeactivateDelayed();
        }
    }

    public void Activate()
    {
        active = true;
        _coroutine = null;
    }

    public void Deactivate()
    {
        active = false;
    }

    private void DeactivateDelayed()
    {
        if (_coroutine != null)
        {
            Clock.Instance().StopCoroutine(_coroutine);
            _coroutine = null;
        }

        _coroutine = DelayDeactivation();
        Clock.Instance().StartCoroutine(_coroutine);
    }

    public IEnumerator DelayDeactivation()
    {
        yield return new WaitForSeconds(_wait_time);
        active = false;
        yield return null;
    }
}