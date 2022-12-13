using UnityEngine;

public class LED_BusBar : LED_Bar
{
    [SerializeField] private Bus _bus = null;

    protected override void Start()
    {
        base.Start();

        if (_bus)
            _value = _bus.value;
    }
}
