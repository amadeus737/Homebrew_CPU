using UnityEngine;

public class MemBridge : MonoBehaviour
{
    [SerializeField] private Bus _memBus = null;
    [SerializeField] private Bus _dataBus = null;

    [SerializeField] private ControlLine _membridge_data_write = null;
    [SerializeField] private ControlLine _membridge_mem_write  = null;

    private void Update()
    {
        if (_membridge_data_write.active)
            _dataBus.RegisterValue(this.GetInstanceID(), _memBus.value);
        else
            _dataBus.UnregisterValue(this.GetInstanceID());

        if (_membridge_mem_write.active)
            _memBus.RegisterValue(this.GetInstanceID(), _dataBus.value);
        else
            _memBus.UnregisterValue(this.GetInstanceID());
    }
}
