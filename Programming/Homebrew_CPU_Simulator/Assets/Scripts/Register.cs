using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Register : MonoBehaviour
{
    [SerializeField] private RegisterLabel _label;
    [SerializeField] private List<ControlledBus> _inBuses = new List<ControlledBus>();
    [SerializeField] private List<ControlledBus> _outBuses = new List<ControlledBus>();
    [SerializeField] private Value _value;
    [SerializeField] private LED_Bar _leds = null;

    public Value value { get { return _value; }}

    private void Start()
    {
        if (_leds != null)
        {
            _leds.SetSource(this.value);
        }
    }

    private void Update()
    {
        // First, deal with input buses
        for (int i = 0; i < _inBuses.Count; i++)
        {            
            if (_inBuses[i].controlLine.active)
            {
                object read_val = _inBuses[i].bus.Read();

                if (!(bool)read_val)
                    _value.state = ValueState.Error;
                else                
                    _value.value = (ulong)read_val;
            }
            else
            {
                _value.state = ValueState.TriState;
            }
        }

        // Now for output buses
        for (int i = 0; i < _outBuses.Count; i++)
        {
            if (_outBuses[i].controlLine.active)
            {
                _outBuses[i].bus.RegisterValue(this.GetInstanceID(), _value);
            }
            else
            {
                _outBuses[i].bus.UnregisterValue(this.GetInstanceID());
            }
        }
    }
}
