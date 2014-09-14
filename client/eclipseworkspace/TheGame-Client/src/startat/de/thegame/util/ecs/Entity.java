package startat.de.thegame.util.ecs;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;


public abstract class Entity {
	private Integer id = null;
	
	private Map<String,ArrayList<Component>> components;
	
	public Entity(Integer id){
		this.id = id;
		components = new HashMap<>();
	}
	
	public void addComponent(Component component){
		ArrayList<Component> list = null;
		
		if((list = components.get(component.getType())) == null){
			list =  new ArrayList<Component>();
			components.put(component.getType(), list);
		}
		
		list.add(component);
		component.setEntity(this);
	}
	
	public ArrayList<Component> getComponents(String type){
		return components.get(type);
	}
	
	public Integer getId() {
		return id;
	}
	
	public abstract void init();
	public abstract void kill();
	
}
